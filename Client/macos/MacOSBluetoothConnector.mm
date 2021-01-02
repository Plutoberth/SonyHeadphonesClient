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

-(void)rfcommChannelOpenComplete:(IOBluetoothRFCOMMChannel *)rfcommChannel status:(IOReturn)error
{
	if ( error != kIOReturnSuccess ) {
		fprintf(stderr,"Error - could not open the RFCOMM channel. Error code = %08x.\n",error);
		return;
	}
	else{
		fprintf(stderr,"Connected. Yeah!\n");
	}
}

-(void)rfcommChannelData:(IOBluetoothRFCOMMChannel *)rfcommChannel data:(void *)dataPointer length:(size_t)dataLength
{
	delegateCPP->receivedBytes = (char*)dataPointer;
	delegateCPP->receivedLength = dataLength;
	delegateCPP->wantNewData = false;
}


@end

int MacOSBluetoothConnector::send(char* buf, size_t length)
{
	int error = [(__bridge IOBluetoothRFCOMMChannel*)rfcommchannel writeSync:(void*)buf length:length];
	if ( error != kIOReturnSuccess ) {
		fprintf(stderr,"Error - could send bluetooth command. Error code = %08x.\n",error);
	}
	return length;
}


void MacOSBluetoothConnector::connectToMac(MacOSBluetoothConnector* macOSBluetoothConnector)
{
	macOSBluetoothConnector->running = 1;
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

	printf("Successfully connected");

	// keep thread running
	while (macOSBluetoothConnector->running) {
		[[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:1]];
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
	wantNewData = true;

	// wait until it has received data
	while (wantNewData) {
		// run the runLoop so it can actually receive data
		[runLoop runUntilDate:[NSDate dateWithTimeIntervalSinceNow:10]];
	}

	// fill the buf with the new data
	for (int n=0;n<receivedLength;n++){
		buf[n] = receivedBytes[n];
	}

	return receivedLength;
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
	running = 0;
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

	printf("closing");
}


bool MacOSBluetoothConnector::isConnected() noexcept
{
	return running;
}
