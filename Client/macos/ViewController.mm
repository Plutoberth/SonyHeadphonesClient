//
//  ViewController.m
//  SonyHeadphonesClient
//
//  Created by Sem Visscher on 01/12/2020.
//

#import "ViewController.h"

@implementation ViewController
@synthesize connectedLabel, connectButton, ANCSlider, ANCValueLabel, focusOnVoice, ANCEnabled, ANCValuePrefixLabel;

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
    NSString *errorText;
    if (exc.shouldDisconnect){
        errorText = @"Unexpected error occurred and disconnected.";
        bt.disconnect();
        [self displayDisconnectedWithText:errorText];
    } else {
        errorText = @"Unexpected error occurred.";
        [connectedLabel setStringValue: errorText];
    }
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:errorText];
    [alert setInformativeText:@(exc.what())];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];

}

- (void)displayDisconnectedWithText: (NSString *)text{
    [ANCSlider setEnabled:FALSE];
    [ANCSlider setIntValue:0];
    [focusOnVoice setEnabled:FALSE];
    [ANCEnabled setEnabled:FALSE];
    [ANCEnabled setState:FALSE];
    [connectButton setTitle:@"Connect to Bluetooth device"];
    [connectedLabel setStringValue:text];
    [ANCValuePrefixLabel setTextColor:NSColor.tertiaryLabelColor];
    [ANCValueLabel setTextColor:NSColor.tertiaryLabelColor];
    statusItem.button.image = [NSImage imageNamed:@"NSRefreshTemplate"];
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
        [self displayDisconnectedWithText:@"Not connected"];
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
            return;
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
            [ANCEnabled setEnabled:TRUE];
            [ANCEnabled setState:TRUE];
            [ANCValuePrefixLabel setTextColor:NSColor.labelColor];
            [ANCValueLabel setTextColor:NSColor.labelColor];
            [focusOnVoice setEnabled:FALSE];
            statusItem.button.image = [NSImage imageNamed:@"NSHomeTemplate"];
        } else {
            [self displayDisconnectedWithText:@"Not connected, connection timed out."];
            bt.disconnect();
        }
    } else {
        [self displayDisconnectedWithText:@"Not connected, device selector canceled."];
    }
    
}

- (IBAction)sendData:(id)sender {
    // check if the device is still connected
    if (!bt.isConnected()) {
        [self displayDisconnectedWithText:@"Not connected, please reconnect."];
        return;
    }
    // change the visuals based on ANCSlider
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
    // change the visuals based on ANCEnabled
    if (ANCEnabled.state){
        [ANCSlider setEnabled:TRUE];
        [ANCValuePrefixLabel setTextColor:NSColor.labelColor];
        [ANCValueLabel setTextColor:NSColor.labelColor];
    }
    else {
        [ANCValuePrefixLabel setTextColor:NSColor.tertiaryLabelColor];
        [ANCValueLabel setTextColor:NSColor.tertiaryLabelColor];
        [focusOnVoice setEnabled:FALSE];
        [ANCSlider setEnabled:FALSE];
    }
    
    // send current settings
    auto ncAsmEffect = ANCEnabled.state ? NC_ASM_EFFECT::ADJUSTMENT_COMPLETION : NC_ASM_EFFECT::OFF;
    auto asmId = focusOnVoice.state == NSControlStateValueOn ? ASM_ID::VOICE : ASM_ID::NORMAL;
    try {
        bt.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
                                                                   ncAsmEffect,
                                                                   NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                                                                   ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                                                                   asmId,
                                                                   ANCEnabled.state ? ANCSlider.intValue : -1
                                                                   ));
    } catch (RecoverableException& exc) {
        [self displayError:exc];
    }
}
@end
