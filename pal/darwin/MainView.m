//
//  MainView.m
//  paldarwin
//
//  Copyright (c) 2015 XSKit.org. All rights reserved.
//

#import "MainView.h"


@implementation MainView

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)drawRect:(NSRect)rect
{
	// setup basics
	[[NSColor grayColor] set];
	NSRectFill ( [self bounds] );
	int count = 12;
	NSRect startingRect = NSMakeRect ( 21,21,50,50 );

	// create arrays of rects and colors
	NSRect    rectArray [count];
	NSColor * colorArray[count];
	rectArray [0] = startingRect;
	colorArray[0] = [NSColor redColor];		

	// populate arrays
	int i;
	NSRect oneRect = rectArray[0];
	for ( i = 1; i < count; i++ )
	{
		// advance to the right
		oneRect.origin.x += 100;

		// if the right edge doesn't fit, move to next row
		if ( NSMaxX (oneRect) > NSMaxX ( [self bounds] ) )
		{
			oneRect.origin.x = startingRect.origin.x;
			oneRect.origin.y += 100;
		}

		rectArray [i] = oneRect;

		// increment color
		colorArray[i] = [NSColor colorWithCalibratedHue: (i*0.04)
                                             saturation: 1
                                             brightness: 0.9
                                                  alpha: 1];
	}

	// use rect and color arrays to fill
	NSRectFillListWithColors ( rectArray, colorArray, count );

	// draw a border around each rect
	[[NSColor whiteColor] set];
	for ( i = 0; i < count; i++) {
		NSFrameRectWithWidth ( rectArray[i], 2 );
	}
}

- (BOOL)isFlipped
{
	return YES;
}

@end
