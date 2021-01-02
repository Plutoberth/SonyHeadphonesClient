//
//  ViewController.m
//  SonyHeadphonesClient
//
//  Created by Sem Visscher on 01/12/2020.
//

#import "ViewController.h"

@implementation ViewController
@synthesize connectedLabel, connectButton, ANCSlider, ANCValueLabel, focusOnVoice, focusOnVoiceLabel;

- (void)viewDidLoad {
    [super viewDidLoad];
    std::unique_ptr<IBluetoothConnector> connector = std::make_unique<MacOSBluetoothConnector>();
    // Wrap the connector using the bluetoothwrapper
    bt = BluetoothWrapper(std::move(connector));
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];
}

- (IBAction)connectToDevice:(id)sender {
    if (bt.isConnected()){
        bt.disconnect();
        [ANCSlider setEnabled:FALSE];
        [focusOnVoice setEnabled:FALSE];
        [connectButton setTitle:@"Connect to Bluetooth device"];
        [connectedLabel setStringValue:@"Not connected"];
    }
    else {
        // launch device selector modal
        IOBluetoothDeviceSelectorController *dSelector = [IOBluetoothDeviceSelectorController deviceSelector];
        int result = [dSelector runModal];
        

        if (result == kIOBluetoothUISuccess) {
            // get device
            IOBluetoothDevice *device = [[dSelector getResults] lastObject];
            // connect to device
            bt.connect([[device addressString] UTF8String]);
            // give it some time to connect
            int timeout = 50;
            while(!bt.isConnected() and timeout >= 0){
                [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
            }
            if (bt.isConnected()){
                IOBluetoothDevice *device = [[dSelector getResults] lastObject];
                [connectedLabel setStringValue:[@"Connected: " stringByAppendingString:[device nameOrAddress]]];
                [connectButton setTitle:@"Disconnect"];
                [ANCSlider setEnabled:TRUE];
                [focusOnVoice setEnabled:FALSE];
            }
            else {
                [connectedLabel setStringValue:@"Not connected, connection timed out."];
            }
        } else {
            [connectedLabel setStringValue:@"Not connected, device selector canceled."];
        }
    }
}

- (IBAction)sliderChanged:(id)sender {
    [ANCValueLabel setStringValue:ANCSlider.stringValue];
    if (ANCSlider.intValue >= MINIMUM_VOICE_FOCUS_STEP)
    {
        [focusOnVoiceLabel setStringValue:@"Focus on Voice"];
        [focusOnVoice setEnabled:TRUE];
    }
    else
    {
        [focusOnVoiceLabel setStringValue:@"Focus on Voice isn't enabled on this level."];
        [focusOnVoice setEnabled:FALSE];
    }
    // send current settings
    auto ncAsmEffect = NC_ASM_EFFECT::ADJUSTMENT_COMPLETION;
    auto asmId = focusOnVoice.state == NSControlStateValueOn ? ASM_ID::VOICE : ASM_ID::NORMAL;
    bt.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
                                                               ncAsmEffect,
                                                               NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                                                               ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                                                               asmId,
                                                               ANCSlider.intValue
                                                               ));
}

- (IBAction)voiceChanged:(id)sender {
    // send current settings
    [ANCValueLabel setStringValue:ANCSlider.stringValue];
    auto ncAsmEffect = NC_ASM_EFFECT::ADJUSTMENT_COMPLETION;
    auto asmId = focusOnVoice.state == NSControlStateValueOn ? ASM_ID::VOICE : ASM_ID::NORMAL;
    bt.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
                                                               ncAsmEffect,
                                                               NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                                                               ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                                                               asmId,
                                                               ANCSlider.intValue
                                                               ));
}

@end
