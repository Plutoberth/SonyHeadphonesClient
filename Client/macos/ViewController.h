//
//  ViewController.h
//  SonyHeadphonesClient
//
//  Created by Sem Visscher on 01/12/2020.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

BluetoothWrapper bt = (BluetoothWrapper)nil;
NSStatusItem* statusItem;

@interface ViewController : NSViewController
@property (weak, nonatomic) IBOutlet NSTextField *connectedLabel;
@property (weak, nonatomic) IBOutlet NSTextField *ANCValuePrefixLabel;
@property (weak, nonatomic) IBOutlet NSTextField *ANCValueLabel;
@property (weak, nonatomic) IBOutlet NSButton *focusOnVoice;
@property (weak, nonatomic) IBOutlet NSButton *connectButton;
@property (weak, nonatomic) IBOutlet NSSlider *ANCSlider;
@property (weak, nonatomic) IBOutlet NSButton *ANCEnabled;
@end

