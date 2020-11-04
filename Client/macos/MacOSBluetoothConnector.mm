// #import <Foundation/Foundation.h>
#include "MacOSBluetoothConnector.h"
#include <thread>

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
    NSString  *message = [[NSString alloc] initWithBytes:dataPointer length:dataLength encoding:NSUTF8StringEncoding];
    delegateCPP->dataRec([message UTF8String]);
}


@end

int MacOSBluetoothConnector::send(char* buf, size_t length)
{
    fprintf(stderr,"Sending Message\n");
    [(__bridge IOBluetoothRFCOMMChannel*)rfcommchannel writeSync:(void*)buf length:length];
	return 1;
}

// doesn't generate error anymore, but doesn't close connection yet, because the channel is not stored globaly 
void MacOSBluetoothConnector::connectToMac(MacOSBluetoothConnector* MacOSBluetoothConnector)
{
	MacOSBluetoothConnector->running = 1;
	//get device
	IOBluetoothDevice *device = (__bridge IOBluetoothDevice *)MacOSBluetoothConnector->rfcommDevice;
	// create new channel
	IOBluetoothRFCOMMChannel *channel = [[IOBluetoothRFCOMMChannel alloc] init];
	// create sppServiceid
	IOBluetoothSDPUUID *sppServiceUUID = [IOBluetoothSDPUUID uuid16: kBluetoothSDPUUID16RFCOMM];
	// get sppServiceRecord
    IOBluetoothSDPServiceRecord *sppServiceRecord = [device getServiceRecordForUUID:sppServiceUUID];
    // get rfcommChannelID from sppServiceRecord
	UInt8 rfcommChannelID;
    [sppServiceRecord getRFCOMMChannelID:&rfcommChannelID];
	// setup delegate
	AsyncCommDelegate* asyncCommDelegate = [[AsyncCommDelegate alloc] init];
	asyncCommDelegate->delegateCPP = MacOSBluetoothConnector;
	// try to open channel
	if ( [device openRFCOMMChannelAsync:&channel withChannelID:rfcommChannelID delegate:asyncCommDelegate] != kIOReturnSuccess ) {
		throw "Error - could not open the rfcomm.\n";
	}
	// store the channel
	MacOSBluetoothConnector->rfcommchannel = (__bridge void*) channel;
	
	printf("Successfully connected");
	// keep thread running
	while (MacOSBluetoothConnector->running) {
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

void MacOSBluetoothConnector::dataRec(const char *dataReceived)
{
    printf("%s\n",dataReceived); // do something more sensible than just printing it! For example establishing a callback here.
}

int MacOSBluetoothConnector::recv(char* buf, size_t length)
{
	return 1;
    // printf("%s\n",dataReceived); // do something more sensible than just printing it! For example establishing a callback here.

}

// currently working
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
	// close the channel
    [chan closeChannel];
	// get the device
	IOBluetoothDevice *device =(__bridge IOBluetoothDevice*) rfcommDevice;
	// disconnect from the device
	[device closeConnection];
    fprintf(stderr,"closing");
}


bool MacOSBluetoothConnector::isConnected() noexcept
{
	return running;
}