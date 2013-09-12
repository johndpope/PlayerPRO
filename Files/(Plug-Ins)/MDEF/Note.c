pascal void main(short msg, MenuHandle whichMenu, Rect *menuRect, Point hitPt, short *itemID );
	#define HIM		14
	#define WIM		27

void GetMDEFRect( Rect *aRect, Rect *menuRect, short whichItem)
{
	short	Xpos, Ypos;
	
	whichItem--;
	
	Xpos = whichItem - (whichItem/12)*12;
	Ypos = whichItem / 12;
	
	aRect->left 	= menuRect->left + Xpos*WIM;
	aRect->right 	= menuRect->left + (Xpos + 1)*WIM;
	
	aRect->top 		= menuRect->top + Ypos*HIM;
	aRect->bottom 	= menuRect->top + (Ypos + 1)*HIM;
}

pascal void main(short msg, MenuHandle whichMenu, Rect *menuRect, Point hitPt, short *itemID )
{
	short		itemCount, j, mouseItem, mSize, Xpos, Ypos;
	Str255		str;
	Rect		tempRect;
	GDHandle	aH;
	RGBColor	theColor;
	
	itemCount = CountMItems( whichMenu);

	TextFont( 0);	TextSize( 0);

	switch (msg)
	{
	case mDrawMsg:
		theColor.red = 56797;
		theColor.green = 56797;
		theColor.blue = 56797;
		
		RGBForeColor( &theColor);
		PaintRect( menuRect);
		
		ForeColor( blackColor);
		RGBBackColor( &theColor);
		
		for ( j = 1;  j <= itemCount;  j++)
		{
			GetMenuItemText( whichMenu, j, str);
			tempRect.bottom--;
			GetMDEFRect( &tempRect, menuRect, j);
			
			if( (*whichMenu)->menuID == j) ForeColor( redColor);
			else ForeColor( blackColor);
			TETextBox( str+1, str[ 0], &tempRect, teJustCenter);
		}
		
		ForeColor( blackColor);
	break;

	case mChooseMsg:
		if( !PtInRect( hitPt, menuRect)) mouseItem = 0;
		else
		{
			mouseItem = (hitPt.v - menuRect->top) / HIM;
			mouseItem *= 12;
			
			mouseItem += (hitPt.h - menuRect->left) / WIM;
			mouseItem++;
			
			if( mouseItem >= itemCount) mouseItem = itemCount;
		}
		
		if (mouseItem == 0)
		{		/* out of bounds or disabled */
			GetMDEFRect( &tempRect, menuRect, *itemID);
			InvertRect( &tempRect);
			*itemID = 0;						/* return "cancel" code */
		}
		else if ( mouseItem != *itemID )
		{
			GetMDEFRect( &tempRect, menuRect, *itemID);
			InvertRect( &tempRect);
			
			GetMDEFRect( &tempRect, menuRect, mouseItem);
			InvertRect( &tempRect);
			*itemID = mouseItem; 				/* return new */
		}
		break;

	case mSizeMsg:
		mSize = 0;
		for ( j=1;  j<=itemCount;  j++ )
		{
			GetMenuItemText( whichMenu, j, str);
			if( StringWidth( str) > mSize) mSize = StringWidth( str);
		}
		(*whichMenu)->menuWidth  = mSize + 2;
		(*whichMenu)->menuHeight =itemCount*HIM; /* assume 12 pts high */
		break;

	case mPopUpMsg:
		
		(*whichMenu)->menuID = *itemID;
		
		Xpos = (*itemID - 1) - ((*itemID - 1)/12)*12;
		Ypos = (*itemID - 1) / 12;
		
		menuRect->top = hitPt.h - (Ypos * HIM);
		menuRect->left = hitPt.v - (Xpos * WIM);
		
		Ypos = (itemCount - 1) / 12;
		
		menuRect->right = menuRect->left + 12*WIM;
		menuRect->bottom = menuRect->top + (Ypos+1)*HIM;
		
		GetMouse( &hitPt);
		
		aH = GetDeviceList();
		do
		{
			if( aH != NULL)
			{
				if( PtInRect( hitPt, &(*(*aH)->gdPMap)->bounds))
				{
					Rect 		ar = (*(*aH)->gdPMap)->bounds;
					
					if( menuRect->bottom > ar.bottom) 	OffsetRect( menuRect, 0, ar.bottom - menuRect->bottom);
					if( menuRect->top < ar.top)			OffsetRect( menuRect, 0, ar.top - menuRect->top);
					if( menuRect->right > ar.right) 	OffsetRect( menuRect, ar.right - menuRect->right, 0);
					if( menuRect->left < ar.left)		OffsetRect( menuRect, ar.left - menuRect->left, 0);
				}
			}
			
			aH = GetNextDevice( aH);
		}
		while( aH != NULL);
	break;

	}	/* end of switch */
	
	TextFont( 0);	TextSize( 0);

	
}	/* end of myMenuDefFn */