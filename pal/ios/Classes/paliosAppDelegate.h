//
//  paliosAppDelegate.h
//  palios
//
//  Created on 11-7-26.
//  Copyright 2011 XSKit.org. All rights reserved.
//

#import <UIKit/UIKit.h>

@class paliosViewController;

@interface paliosAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    paliosViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet paliosViewController *viewController;

@end

