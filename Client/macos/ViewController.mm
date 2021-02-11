//
//  ViewController.m
//  SonyHeadphonesClient
//
//  Created by Sem Visscher on 01/12/2020.
//

#import "ViewController.h"

@implementation ViewController
@synthesize connectedLabel, connectButton, ANCSlider, ANCValueLabel, focusOnVoice;

- (void)viewDidLoad {
    [super viewDidLoad];
    std::unique_ptr<IBluetoothConnector> connector = std::make_unique<MacOSBluetoothConnector>();
    // Wrap the connector using the bluetoothwrapper
    bt = BluetoothWrapper(std::move(connector));
    statusItem = [NSStatusBar.systemStatusBar statusItemWithLength: -1];
    statusItem.button.image = [NSImage imageNamed:@"NSRefreshTemplate"];
    [statusItem setTarget:self];
    [statusItem setAction:@selector(statusItemClick:)];
}

- (void)displayError:(RecoverableException)exc {
    if (exc.shouldDisconnect){
        bt.disconnect();
        [ANCSlider setEnabled:FALSE];
        [focusOnVoice setEnabled:FALSE];
        [connectButton setTitle:@"Connect to Bluetooth device"];
        statusItem.button.image = [NSImage imageNamed:@"NSRefreshTemplate"];
        [connectedLabel setStringValue:[@"Unexpected error occurred and disconnected: \n" stringByAppendingString:@(exc.what())]];
    } else {
        [connectedLabel setStringValue:[@"Unexpected error occurred: \n" stringByAppendingString:@(exc.what())]];
    }
}

- (void)statusItemClick:(id)sender {
    if (!bt.isConnected())
        return [self connectToDevice:self];
    
    if ([focusOnVoice isEnabled]) {
        [ANCSlider setIntValue:0];
        [focusOnVoice setEnabled:FALSE];
    } else {
        [ANCSlider setIntValue:19];
        [focusOnVoice setEnabled:TRUE];
    }
    [self sendData:self];
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];
}

- (IBAction)connectToDevice:(id)sender {
    statusItem.button.image = [NSImage imageNamed:@"NSRefreshTemplate"];
    
    // check if it should disconnect
    if (bt.isConnected()) {
        bt.disconnect();
        [ANCSlider setEnabled:FALSE];
        [focusOnVoice setEnabled:FALSE];
        [connectButton setTitle:@"Connect to Bluetooth device"];
        [connectedLabel setStringValue:@"Not connected"];
        return;
    }
    
    // launch device selector modal
    IOBluetoothDeviceSelectorController *dSelector = [IOBluetoothDeviceSelectorController deviceSelector];
    int result = [dSelector runModal];
    
    
    if (result == kIOBluetoothUISuccess) {
        // get device
        IOBluetoothDevice *device = [[dSelector getResults] lastObject];
        try {
            // connect to device
            bt.connect([[device addressString] UTF8String]);
        } catch (RecoverableException& exc) {
            [self displayError:exc];
        }
        // give it some time to connect
        int timeout = 5;
        while(!bt.isConnected() and timeout >= 0) {
            [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
            timeout--;
        }
        if (bt.isConnected()) {
            IOBluetoothDevice *device = [[dSelector getResults] lastObject];
            [connectedLabel setStringValue:[@"Connected: " stringByAppendingString:[device nameOrAddress]]];
            [connectButton setTitle:@"Disconnect"];
            [ANCSlider setEnabled:TRUE];
            [focusOnVoice setEnabled:FALSE];
            statusItem.button.image = [NSImage imageNamed:@"NSHomeTemplate"];
        } else {
            [connectedLabel setStringValue:@"Not connected, connection timed out."];
            bt.disconnect();
        }
    } else {
        [connectedLabel setStringValue:@"Not connected, device selector canceled."];
    }
    
}

- (IBAction)sendData:(id)sender {
    // check if the device is still connected
    if (!bt.isConnected()) {
        [ANCSlider setEnabled:FALSE];
        [ANCSlider setIntValue:0];
        [focusOnVoice setEnabled:FALSE];
        [connectButton setTitle:@"Connect to Bluetooth device"];
        [connectedLabel setStringValue:@"Not connected, please reconnect."];
        statusItem.button.image = [NSImage imageNamed:@"NSRefreshTemplate"];
        return;
    }
    
    [ANCValueLabel setStringValue:ANCSlider.stringValue];
    if (ANCSlider.intValue >= MINIMUM_VOICE_FOCUS_STEP) {
        [focusOnVoice setTitle:@"Focus on Voice"];
        [focusOnVoice setEnabled:TRUE];
        statusItem.button.image = [NSImage imageNamed:@"NSFlowViewTemplate"];
    }
    else {
        [focusOnVoice setTitle:@"Focus on Voice isn't enabled on this level."];
        statusItem.button.image = [NSImage imageNamed:@"NSHomeTemplate"];
        [focusOnVoice setEnabled:FALSE];
    }
    
    // send current settings
    auto ncAsmEffect = NC_ASM_EFFECT::ADJUSTMENT_COMPLETION;
    auto asmId = focusOnVoice.state == NSControlStateValueOn ? ASM_ID::VOICE : ASM_ID::NORMAL;
    try {
        bt.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
                                                                   ncAsmEffect,
                                                                   NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                                                                   ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                                                                   asmId,
                                                                   ANCSlider.intValue
                                                                   ));
    } catch (RecoverableException& exc) {
        [self displayError:exc];
    }
}
@end
