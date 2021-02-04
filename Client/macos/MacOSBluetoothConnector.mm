#include "MacOSBluetoothConnector.h"

MacOSBluetoothConnector::MacOSBluetoothConnector()
{
    
}
MacOSBluetoothConnector::~MacOSBluetoothConnector()
{
    //onclose event
    if (isConnected()){
        disconnect();
    }
}

@interface AsyncCommDelegate : NSObject <IOBluetoothRFCOMMChannelDelegate> {
@public
    MacOSBluetoothConnector* delegateCPP;
}
@end

@implementation AsyncCommDelegate {
}
- (void)rfcommChannelClosed:(IOBluetoothRFCOMMChannel *)rfcommChannel{
    delegateCPP->disconnect();
}

-(void)rfcommChannelData:(IOBluetoothRFCOMMChannel *)rfcommChannel data:(void *)dataPointer length:(size_t)dataLength
{
    std::lock_guard<std::mutex> g(delegateCPP->receiveDataMutex);
    
    unsigned char* buffer = (unsigned char*)dataPointer;
    std::vector<unsigned char> vectorBuffer(buffer, buffer+dataLength);
    
    delegateCPP->receivedBytes.push_back(vectorBuffer);
    delegateCPP->receiveDataConditionVariable.notify_one();
}


@end

int MacOSBluetoothConnector::send(char* buf, size_t length)
{
    [(__bridge IOBluetoothRFCOMMChannel*)rfcommchannel writeSync:(void*)buf length:length];
    return (int)length;
}


void MacOSBluetoothConnector::connectToMac(MacOSBluetoothConnector* macOSBluetoothConnector)
{
    macOSBluetoothConnector->running = true;
    //get device
    IOBluetoothDevice *device = (__bridge IOBluetoothDevice *)macOSBluetoothConnector->rfcommDevice;
    // create new channel
    IOBluetoothRFCOMMChannel *channel = [[IOBluetoothRFCOMMChannel alloc] init];
    // create sppServiceid
    IOBluetoothSDPUUID *sppServiceUUID = [IOBluetoothSDPUUID uuidWithBytes:(void*)SERVICE_UUID_IN_BYTES length: 16];
    // get sppServiceRecord
    IOBluetoothSDPServiceRecord *sppServiceRecord = [device getServiceRecordForUUID:sppServiceUUID];
    // get rfcommChannelID from sppServiceRecord
    UInt8 rfcommChannelID;
    [sppServiceRecord getRFCOMMChannelID:&rfcommChannelID];
    // setup delegate
    AsyncCommDelegate* asyncCommDelegate = [[AsyncCommDelegate alloc] init];
    asyncCommDelegate->delegateCPP = macOSBluetoothConnector;
    // try to open channel
    if ( [device openRFCOMMChannelAsync:&channel withChannelID:rfcommChannelID delegate:asyncCommDelegate] != kIOReturnSuccess ) {
        throw "Error - could not open the rfcomm.\n";
    }
    // store the channel
    macOSBluetoothConnector->rfcommchannel = (__bridge void*) channel;
    
    // keep thread running
    while (macOSBluetoothConnector->running) {
        [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:.1]];
    }
}
void MacOSBluetoothConnector::connect(const std::string& addrStr){
    // convert mac adress to nsstring
    NSString *addressNSString = [NSString stringWithCString:addrStr.c_str() encoding:[NSString defaultCStringEncoding]];
    // get device based on mac adress
    IOBluetoothDevice *device = [IOBluetoothDevice deviceWithAddressString:addressNSString];
    // if device is not connected
    if (![device isConnected]) {
        [device openConnection];
    }
    // store the device in an variable
    rfcommDevice = (__bridge void*) device;
    uthread = new std::thread(MacOSBluetoothConnector::connectToMac, this);
}

int MacOSBluetoothConnector::recv(char* buf, size_t length)
{
    // wait for newly received data
    std::unique_lock<std::mutex> g(receiveDataMutex);
    receiveDataConditionVariable.wait(g, [this]{ return !receivedBytes.empty(); });
    
    // fill the buf with the new data
    std::vector<unsigned char> receivedVector = receivedBytes.front();
    receivedBytes.pop_front();
    
    size_t lengthCopied = std::min(length, receivedVector.size());

    // copy the first amount of bytes
    std::memcpy(buf, receivedVector.data(), lengthCopied);
    
    // too much data, save it for next time
    if (receivedVector.size() > length){
        receivedVector.erase(receivedVector.begin(), receivedVector.begin() + lengthCopied);
        receivedBytes.push_front(receivedVector);
    }
    
    return (int)lengthCopied;
}

std::vector<BluetoothDevice> MacOSBluetoothConnector::getConnectedDevices()
{
    // create the output vector
    std::vector<BluetoothDevice> res;
    // loop through the paired devices (also includes non paired devices for some reason)
    for (IOBluetoothDevice* device in [IOBluetoothDevice pairedDevices]) {
        // check if device is connected
        if ([device isConnected]) {
            BluetoothDevice dev;
            // save the mac address and name
            dev.mac =  [[device addressString]UTF8String];
            dev.name = [[device name] UTF8String];
            // add device to the connected devices vector
            res.push_back(dev);
        }
    }
    
    return res;
}

void MacOSBluetoothConnector::disconnect() noexcept
{
    running = false;
    // wait for the thread to finish
    uthread->join();
    // close connection
    closeConnection();
}
void MacOSBluetoothConnector::closeConnection() {
    // get the channel
    IOBluetoothRFCOMMChannel *chan = (__bridge IOBluetoothRFCOMMChannel*) rfcommchannel;
    [chan setDelegate: nil];
    // close the channel
    [chan closeChannel];
}


bool MacOSBluetoothConnector::isConnected() noexcept
{
    IOBluetoothRFCOMMChannel *chan = (__bridge IOBluetoothRFCOMMChannel*) rfcommchannel;
    return running and chan.isOpen;
}
