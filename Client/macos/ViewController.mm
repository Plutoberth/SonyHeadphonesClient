//
//  ViewController.m
//  SonyHeadphonesClient
//
//  Created by Sem Visscher on 01/12/2020.
//

#import "ViewController.h"

@implementation ViewController
@synthesize connectedLabel, connectButton, ANCSlider, ANCValueLabel, focusOnVoice, ANCEnabled, ANCValuePrefixLabel, virtualSoundLabel, soundPositionLabel, surroundLabel, soundPosition, surround;

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
    [focusOnVoice setEnabled:FALSE];
    [surround setEnabled:FALSE];
    [soundPosition setEnabled:FALSE];
    [virtualSoundLabel setTextColor:NSColor.tertiaryLabelColor];
    [surroundLabel setTextColor:NSColor.tertiaryLabelColor];
    [soundPositionLabel setTextColor:NSColor.tertiaryLabelColor];
    [ANCValuePrefixLabel setTextColor:NSColor.tertiaryLabelColor];
    [ANCValueLabel setTextColor:NSColor.tertiaryLabelColor];
    [connectedLabel setStringValue:text];
    [surround selectItemAtIndex:0];
    [soundPosition selectItemAtIndex:0];
    [connectButton setTitle:@"Connect to Bluetooth device"];
    statusItem.button.image = [NSImage imageNamed:@"NSRefreshTemplate"];
}

- (void)statusItemClick:(id)sender {   
    headphones->setAmbientSoundControl(TRUE);
    if ([focusOnVoice isEnabled]) {
        [ANCSlider setIntValue:0];
        headphones->setAsmLevel(0);
        [focusOnVoice setEnabled:FALSE];
    } else {
        [ANCSlider setIntValue:19];
        headphones->setAsmLevel(19);
        [focusOnVoice setEnabled:TRUE];
    }
    
    [self updateHeadphones];
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
            [focusOnVoice setEnabled:FALSE];
            [surround setEnabled:TRUE];
            [soundPosition setEnabled:TRUE];
            [virtualSoundLabel setTextColor:NSColor.labelColor];
            [surroundLabel setTextColor:NSColor.labelColor];
            [soundPositionLabel setTextColor:NSColor.labelColor];
            [ANCValuePrefixLabel setTextColor:NSColor.labelColor];
            [ANCValueLabel setTextColor:NSColor.labelColor];
            statusItem.button.image = [NSImage imageNamed:@"NSHomeTemplate"];
            headphones = new Headphones(bt);
        } else {
            [self displayDisconnectedWithText:@"Not connected, connection timed out."];
            bt.disconnect();
        }
    } else {
        [self displayDisconnectedWithText:@"Not connected, device selector canceled."];
    }
}

- (IBAction)ANCSliderChanged:(id)sender {
    headphones->setAmbientSoundControl(TRUE);
    headphones->setAsmLevel(ANCSlider.intValue);
    [self updateHeadphones];
}

- (IBAction)ANCEnabledButtonChanged:(id)sender {
    headphones->setAmbientSoundControl(ANCEnabled.state);
    [self updateHeadphones];
}

- (IBAction)focusOnVoiceChanged:(id)sender {
    headphones->setFocusOnVoice(focusOnVoice.state);
    [self updateHeadphones];
}

- (IBAction)surroundChanged:(id)sender {
    headphones->setVptType((int) surround.indexOfSelectedItem);
    headphones->setSurroundPosition(SOUND_POSITION_PRESET_ARRAY[0]);
    [self updateHeadphones];
}

- (IBAction)soundPositionChanged:(id)sender {
    headphones->setVptType(0);
    headphones->setSurroundPosition(SOUND_POSITION_PRESET_ARRAY[soundPosition.indexOfSelectedItem]);
    [self updateHeadphones];
}

- (void)updateHeadphones {
    if (!bt.isConnected()) {
        [self displayDisconnectedWithText:@"Not connected, please reconnect."];
        return;
    }
    if (headphones->isChanged()) {
        // run in a background thread
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
            try {
                headphones->setChanges();
                [self updateGUI];
            } catch (RecoverableException& exc) {
                [self displayError:exc];
            }
        });
    }   
}

- (void)updateGUI {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self->ANCEnabled setState:headphones->getAmbientSoundControl()];
        [self->focusOnVoice setState:headphones->getFocusOnVoice()];
        [self->surround selectItemAtIndex:headphones->getVptType()];
        SOUND_POSITION_PRESET preset = headphones->getSurroundPosition();
        // get the index of the preset, from the SOUND_POSITION_PRESET_ARRAY,
        int index = 0;
        for (SOUND_POSITION_PRESET p : SOUND_POSITION_PRESET_ARRAY) {
            if (p == preset) {
                break;
            }
            index++;
        }
        [self->soundPosition selectItemAtIndex:index];
        if (headphones->isSetAsmLevelAvailable()){
            [self->ANCSlider setEnabled:TRUE];
            [self->ANCValuePrefixLabel setTextColor:NSColor.labelColor];
            [self->ANCValueLabel setTextColor:NSColor.labelColor];
        }
        else {
            [self->ANCValuePrefixLabel setTextColor:NSColor.tertiaryLabelColor];
            [self->ANCValueLabel setTextColor:NSColor.tertiaryLabelColor];
            [self->focusOnVoice setEnabled:FALSE];
            [self->ANCSlider setEnabled:FALSE];
        }

        [self->ANCValueLabel setIntValue:headphones->getAsmLevel()];
        [self->ANCSlider setIntValue:headphones->getAsmLevel()];
        if (headphones->isFocusOnVoiceAvailable()) {
            [self->focusOnVoice setTitle:@"Focus on Voice"];
            [self->focusOnVoice setEnabled:TRUE];
            statusItem.button.image = [NSImage imageNamed:@"NSFlowViewTemplate"];
        }
        else {
            [self->focusOnVoice setTitle:@"Focus on Voice isn't enabled on this level."];
            statusItem.button.image = [NSImage imageNamed:@"NSHomeTemplate"];
            [self->focusOnVoice setEnabled:FALSE];
        }
    });
}


@end
