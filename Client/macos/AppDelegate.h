//
//  AppDelegate.h
//  SonyHeadphonesClient
//
//  Created by Sem Visscher on 01/12/2020.
//

#import <Cocoa/Cocoa.h>
#import <IOBluetoothUI/IOBluetoothUI.h>
#import <stdio.h>
#import "BluetoothWrapper.h"
#import "Headphones.h"
#import "MacOSBluetoothConnector.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (weak) NSWindow* window;
@end
