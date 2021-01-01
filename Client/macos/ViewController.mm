//
//  ViewController.m
//  SonyHeadphonesClient
//
//  Created by Sem Visscher on 01/12/2020.
//

#import "ViewController.h"

@implementation ViewController
@synthesize connectedLabel, connectButton, ANCSlider, ANCValueLabel;

- (void)viewDidLoad {
    [super viewDidLoad];
    std::unique_ptr<IBluetoothConnector> connector = std::make_unique<MacOSBluetoothConnector>();
    // Wrap the connector using the bluetoothwrapper
    bt = BluetoothWrapper(std::move(connector));
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (IBAction)connectToDevice:(id)sender {
    if (bt.isConnected()){
        bt.disconnect();
        [ANCSlider setEnabled:FALSE];
        [connectButton setTitle:@"Connect to Bluetooth device"];
        [connectedLabel setStringValue:@"Not connected"];
    }
    else {
        IOBluetoothDeviceSelectorController *dSelector = [IOBluetoothDeviceSelectorController deviceSelector];
        int result = [dSelector runModal];
        if (result == kIOBluetoothUISuccess) {
            IOBluetoothDevice *device = [[dSelector getResults] lastObject];
            NSLog(@"%@", [device addressString]);
            bt.connect([[device addressString] UTF8String]);
        }
        int timeout = 20;
        while(!bt.isConnected() and timeout >= 0){
            [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
        }
        if (bt.isConnected()){
            IOBluetoothDevice *device = [[dSelector getResults] lastObject];
            [connectedLabel setStringValue:[@"Connected: " stringByAppendingString:[device nameOrAddress]]];
            [connectButton setTitle:@"Disconnect"];
            [ANCSlider setEnabled:TRUE];
        }
    }
}
- (IBAction)sliderChanged:(id)sender {
    NSLog(@"%@", ANCSlider.stringValue);
    [ANCValueLabel setStringValue:ANCSlider.stringValue];
}

@end
