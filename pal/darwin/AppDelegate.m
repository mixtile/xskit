//
//  AppDelegate.m
//  paldarwin
//
//  Created by Martin on 7/14/15.
//  Copyright (c) 2015 XSKit. All rights reserved.
//

#import "AppDelegate.h"

#import <Cocoa/Cocoa.h>

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
//    [[[self window] contentView] setNeedsDisplay];
    printf("Hello");
    NSLog(@"Context: %d", UIGraphicsGetCurrentContext());
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
