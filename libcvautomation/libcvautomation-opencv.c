/*
 * =====================================================================================
 *
 *       Filename:  libcvautomation-opencv.c
 *
 *    Description:  
 *
 *        Created:  06/21/2012 08:34:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bradlee Speice, bspeice@uncc.edu
 *   Organization:  MOSAIC at University of North Carolina at Charlotte
 *
 * =====================================================================================
 */

#include <libcvautomation/libcvautomation-opencv.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  matchSubImage
 *      Returns:  CvPoint if a match is found, null if not
 * =====================================================================================
 */

cvaPoint matchSubImage ( IplImage *rootImage, IplImage *subImage, int searchMethod, double tolerance )
{
	/* We have the two OpenCV images we want, go ahead and find if there are any matches */
	IplImage	*result;
	CvPoint		minloc, maxloc; /* Location for the match - depending on search algorithm,
									the result may be in either minloc or maxloc */
	cvaPoint	returnPoint; /* (-1, -1), used to indicate an error */
	double		minval, maxval;
	int			rootImage_width, rootImage_height;
	int			subImage_width, subImage_height;
	int			result_width, result_height;

	returnPoint.x = returnPoint.y = -1;

	/* Make sure we have valid images */
	if ( rootImage == 0 || subImage == 0) {
		/* Otherwise return invalid */
		return returnPoint;
	}

	/* Set up the parameters for our result image */
	rootImage_width		= rootImage->width;
	rootImage_height	= rootImage->height;
	subImage_width	= subImage->width;
	subImage_height	= subImage->height;
	result_width	= rootImage_width - subImage_width + 1;
	result_height	= rootImage_height - subImage_height + 1;

	/* Initialize our result image */
	result = cvCreateImage( cvSize( result_width, result_height ), IPL_DEPTH_32F, 1 );

	/* Compute the result image */
	cvMatchTemplate( rootImage, subImage, result, searchMethod );
	cvMinMaxLoc( result, &minval, &maxval, &minloc, &maxloc, 0 );

	/* Free memory for the result image
	 * Note that we don't control the root or sub image,
	 * so don't mess with those */
	cvReleaseImage( &result );

	/* Make sure that we have enough correlation to return a result,
	 * and then return the match location*/
	/* Return the match location */
	if ( searchMethod == CV_TM_SQDIFF || searchMethod == CV_TM_SQDIFF_NORMED )
	{
		if ( minval < tolerance )
		{
			returnPoint.x = minloc.x;
			returnPoint.y = minloc.y;
			return returnPoint;
		}
		else
			/* Return invalid */
			return returnPoint;
	}
	else
	{
		if ( maxval > tolerance )
		{
			returnPoint.x = maxloc.x;
			returnPoint.y = maxloc.y;
			return returnPoint;
		}
		else
			/* Return invalid */
			return returnPoint;
	}

}		/* -----  end of function matchSubImage  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  matchSubImage_location
 *  Description:  Match a root image and sub image from filename
 * =====================================================================================
 */
cvaPoint matchSubImage_location ( const char *rootImage_location, const char *subImage_location, int searchMethod, double tolerance )
{
	/* This is basically a wrapper for matchSubImage( IplImage, IplImage )
	 * All we do is load the images from the given filenames, and then
	 * pass off the result to the above-named function */
	IplImage *rootImage;
	rootImage = cvLoadImage( rootImage_location, CV_LOAD_IMAGE_COLOR );

	IplImage *subImage;
	subImage = cvLoadImage( subImage_location, CV_LOAD_IMAGE_COLOR );

	cvaPoint resultPoint;
	resultPoint.x = resultPoint.y = -1;

	/* Make sure we have good images */
	if ( rootImage == 0 || subImage == 0 )
	{
		/* Return error */
		return resultPoint;
	}

	resultPoint = matchSubImage( rootImage, subImage, searchMethod, tolerance );

	/* Free up the memory we created */
	cvReleaseImage( &rootImage );
	cvReleaseImage( &subImage );

	/* Our resultPoint will already be invalid if there's no match */
	return resultPoint;

}		/* -----  end of function matchSubImage  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  matchSubImage_a
 *  Description:  Match a root image and sub image from an array of sub-images.
 *  				The list contains an element for each sub-image to specify its own
 *  				searchMethod and threshold value.
 * =====================================================================================
 */
void matchSubImage_a ( IplImage *rootImage, cvautomationList *subImageArray, int listSize )
{
	/* This is also a higher-end wrapper for matchSubImage, but is mostly aimed
	 * at making python support for multiple images very easy. */

	cvaPoint resultPoint;
	cvautomationList curr;

	int x = 0;
	for ( x = 0; x < listSize; x++ )
	{
		curr = subImageArray[x];
		if ( subImageArray[x].cvaImage != 0 )
			resultPoint = matchSubImage ( rootImage, curr.cvaImage, curr.searchMethod, curr.tolerance );
		else
		{
			/* We have a sub-image filename, go ahead and create the actual image. */
			IplImage *subImage;
			subImage = cvLoadImage( curr.fileName, CV_LOAD_IMAGE_COLOR );

			resultPoint = matchSubImage ( rootImage, subImage, curr.searchMethod, curr.tolerance );

			cvReleaseImage( &subImage );
		}

		curr.resultPoint = resultPoint;
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  matchSubImage_a_location
 *  Description:  Match a root image from location, and sub image from
 *  				an array of sub-images.
 *  				The list contains an element for each sub-image to specify its own
 *  				searchMethod and threshold value.
 * =====================================================================================
 */
void matchSubImage_a_location ( const char *rootImage_location, cvautomationList *subImageArray, int listSize )
{
	/* This is also a higher-end wrapper for matchSubImage, but is mostly aimed
	 * at making python support for multiple images very easy. */

	cvaPoint resultPoint;
	cvautomationList curr;

	IplImage *rootImage;
	rootImage = cvLoadImage( rootImage_location, CV_LOAD_IMAGE_COLOR );

	int x = 0;
	for ( x = 0; x < listSize; x++ )
	{
		curr = subImageArray[x];
		if ( subImageArray[x].cvaImage != 0 )
			resultPoint = matchSubImage ( rootImage, curr.cvaImage, curr.searchMethod, curr.tolerance );
		else
		{
			/* We have a sub-image filename, go ahead and create the actual image. */
			IplImage *subImage;
			subImage = cvLoadImage( curr.fileName, CV_LOAD_IMAGE_COLOR );

			resultPoint = matchSubImage ( rootImage, subImage, curr.searchMethod, curr.tolerance );

			cvReleaseImage( &subImage );
		}

		curr.resultPoint = resultPoint;
	}

	cvReleaseImage( &rootImage );
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  matchSubImage_center
 *      Returns:  CvPoint if a match is found, null if not
 *      			The code is mostly the same as matchSubImage, but the difference
 *      			here is that it returns the center of the sub-image, rather than
 *      			the top-left corner.
 * =====================================================================================
 */

cvaPoint matchSubImage_center ( IplImage *rootImage, IplImage *subImage, int searchMethod, double tolerance )
{
	/* We have the two OpenCV images we want, go ahead and find if there are any matches */
	IplImage	*result;
	CvPoint		minloc, maxloc; /* Location for the match - depending on search algorithm,
									the result may be in either minloc or maxloc */
	cvaPoint	returnPoint; /* (-1, -1), used to indicate an error */
	double		minval, maxval;
	int			rootImage_width, rootImage_height;
	int			subImage_width, subImage_height;
	int			result_width, result_height;

	returnPoint.x = returnPoint.y = -1;

	/* Make sure we have valid images */
	if ( rootImage == 0 || subImage == 0) {
		/* Otherwise return invalid */
		return returnPoint;
	}

	/* Set up the parameters for our result image */
	rootImage_width		= rootImage->width;
	rootImage_height	= rootImage->height;
	subImage_width	= subImage->width;
	subImage_height	= subImage->height;
	result_width	= rootImage_width - subImage_width + 1;
	result_height	= rootImage_height - subImage_height + 1;

	/* Initialize our result image */
	result = cvCreateImage( cvSize( result_width, result_height ), IPL_DEPTH_32F, 1 );

	/* Compute the result image */
	cvMatchTemplate( rootImage, subImage, result, searchMethod );
	cvMinMaxLoc( result, &minval, &maxval, &minloc, &maxloc, 0 );

	/* Free memory for the result image
	 * Note that we don't control the root or sub image,
	 * so don't mess with those */
	cvReleaseImage( &result );

	/* Make sure that we have enough correlation to return a result,
	 * and then return the match location*/
	/* Return the match location */
	if ( searchMethod == CV_TM_SQDIFF || searchMethod == CV_TM_SQDIFF_NORMED )
	{
		if ( minval < tolerance )
		{
			returnPoint.x = minloc.x;
			returnPoint.y = minloc.y;
			/* Center the image before returning */
			returnPoint.x += (subImage->width) / 2;
			returnPoint.y += (subImage->height) / 2;

			return returnPoint;
		}
		else
			/* Return invalid */
			return returnPoint;
	}
	else
	{
		if ( maxval > tolerance )
		{
			returnPoint.x = maxloc.x;
			returnPoint.y = maxloc.y;
			/* Center the image before returning */
			returnPoint.x += (subImage->width) / 2;
			returnPoint.y += (subImage->height) / 2;

			return returnPoint;
		}
		else
			/* Return invalid */
			return returnPoint;
	}

}		/* -----  end of function matchSubImage_center  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  matchSubImage_location_center
 *  Description:  Match a root image and sub image from filename
 *  				Most of the code is the same as matchSubImage_location, but use
 *  				matchSubImage_center as the lower-level call to make sure that
 *  				we get the center of the sub-image.
 * =====================================================================================
 */
cvaPoint matchSubImage_location_center ( const char *rootImage_location, const char *subImage_location, int searchMethod, double tolerance )
{
	/* This is basically a wrapper for matchSubImage( IplImage, IplImage )
	 * All we do is load the images from the given filenames, and then
	 * pass off the result to the above-named function */
	IplImage *rootImage;
	rootImage = cvLoadImage( rootImage_location, CV_LOAD_IMAGE_COLOR );

	IplImage *subImage;
	subImage = cvLoadImage( subImage_location, CV_LOAD_IMAGE_COLOR );

	cvaPoint resultPoint;
	resultPoint.x = resultPoint.y = -1;

	/* Make sure we have good images */
	if ( rootImage == 0 || subImage == 0 )
	{
		/* Return error */
		return resultPoint;
	}

	resultPoint = matchSubImage_center( rootImage, subImage, searchMethod, tolerance );

	/* Free up the memory we created */
	cvReleaseImage( &rootImage );
	cvReleaseImage( &subImage );

	/* Our return_point will already be NULL if there's no match */
	return resultPoint;

}		/* -----  end of function matchSubImage_location_center  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  matchSubImage_a_center
 *  Description:  Match a root image and sub image from an array of sub-images.
 *  				The list contains an element for each sub-image to specify its own
 *  				searchMethod and threshold value.
 *  				Most of the code is the same as matchSubImage_a, but we use
 *  				matchSubImage_center as the underlying function to make sure we
 *  				get the center of the sub-image, rather than the top-left corner
 * =====================================================================================
 */
void matchSubImage_a_center ( IplImage *rootImage, cvautomationList *subImageArray, int listSize )
{
	/* This is also a higher-end wrapper for matchSubImage, but is mostly aimed
	 * at making python support for multiple images very easy. */

	cvaPoint resultPoint;
	cvautomationList curr;

	int x = 0;
	for ( x = 0; x < listSize; x++ )
	{
		curr = subImageArray[x];
		if ( subImageArray[x].cvaImage != 0 )
			resultPoint = matchSubImage ( rootImage, curr.cvaImage, curr.searchMethod, curr.tolerance );
		else
		{
			/* We have a sub-image filename, go ahead and create the actual image. */
			IplImage *subImage;
			subImage = cvLoadImage( curr.fileName, CV_LOAD_IMAGE_COLOR );

			resultPoint = matchSubImage_center ( rootImage, subImage, curr.searchMethod, curr.tolerance );

			cvReleaseImage( &subImage );
		}

		curr.resultPoint = resultPoint;
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  matchSubImage_a_location_center
 *  Description:  Match a root image from location, and sub image from
 *  				an array of sub-images.
 *  				The list contains an element for each sub-image to specify its own
 *  				searchMethod and threshold value.
 *  				Most of the code is the same as matchSubImage_a_location, but we
 *  				use matchSubImage_center in order to return the center of the sub-
 *  				image, rather than the top-left corner.
 * =====================================================================================
 */
void matchSubImage_a_location_center ( const char *rootImage_location, cvautomationList *subImageArray, int listSize )
{
	/* This is also a higher-end wrapper for matchSubImage, but is mostly aimed
	 * at making python support for multiple images very easy. */

	cvaPoint resultPoint;
	cvautomationList curr;

	IplImage *rootImage;
	rootImage = cvLoadImage( rootImage_location, CV_LOAD_IMAGE_COLOR );

	int x = 0;
	for ( x = 0; x < listSize; x++ )
	{
		curr = subImageArray[x];
		if ( subImageArray[x].cvaImage != 0 )
			resultPoint = matchSubImage ( rootImage, curr.cvaImage, curr.searchMethod, curr.tolerance );
		else
		{
			/* We have a sub-image filename, go ahead and create the actual image. */
			IplImage *subImage;
			subImage = cvLoadImage( curr.fileName, CV_LOAD_IMAGE_COLOR );

			resultPoint = matchSubImage_center ( rootImage, subImage, curr.searchMethod, curr.tolerance );

			cvReleaseImage( &subImage );
		}

		curr.resultPoint = resultPoint;
	}

	cvReleaseImage( &rootImage );
}

