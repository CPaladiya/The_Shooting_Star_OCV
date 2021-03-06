#include <opencv2/imgcodecs.hpp> //this is for reading files
#include <opencv2/highgui.hpp> //this is for basic gui
#include <opencv2/imgproc.hpp> //for image processing
#include <iostream>

using namespace cv;
using namespace std;


//---------------------------Project 1 : Realtime Marker Detection ------------------//

// we will have multiple masks for multiple colors we want to detect, for multiple colors we will use vector

//vector of Hmin,Hmax,Smin,Smax,Vmin,Vmanx values for two different colors
//OpenCV Uses BGR color definition instead of RGB
vector<vector<int>> myColors {{45,97,65,255,105,255},{94,128,35,255,129,230}}; //bright green and blue here


Mat img; 
vector<vector<int>> newPointsColor1; //ecah one will store {{x of circle ,y of circle ,color 0 = blue/1 = green}}
vector<vector<int>> newPointsColor2; //separate matrix for each color identified

Point getContours(Mat &imgDil){

    //here is how to define vectors of countours, they are list of points within list so a vector inside a vector of a Points
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy; //besically 4 integer values. However we have not used this here much in this example

    findContours(imgDil, contours, hierarchy,RETR_LIST,CHAIN_APPROX_SIMPLE);

    //this will be exacly like contours, but it will only have corner points
    vector<vector<Point>> conPoly(contours.size());
    //creating a bounding box point values using conPoly
    vector<Rect> boundRect(contours.size()); //note the data type here
    Point myPoint(0,0);

    for (int i = 0; i< contours.size(); i++){
        
        double area = contourArea(contours[i]);
        cout << "Area of the i "<< i << " contour : "<< area << endl;
        //next we can create a filter where we only identify area if it is more than 1000

        //object type
        string objType;

        if (area > 1000){
            
            float peri = arcLength(contours[i], true); //this will be the bounding box around the object

            approxPolyDP(contours[i], conPoly[i], 0.02*peri, true);

            //to get the idea of number of contours and size
            cout << "Number of contor on i " << i << " :" << conPoly[i].size() << endl;

            //these two line will help us draw the square contour around the object we identified
            boundRect[i] = boundingRect(conPoly[i]);
            //we are drawing a circle from middle of the top line of bouding rectangle, so start of x + half way of top line
            myPoint.x = boundRect[i].x + boundRect[i].width/2;
            myPoint.y = boundRect[i].y;


            //drawContours(img,conPoly,i,Scalar(255,0,255),1);
            //creating bounding box around the detected contour
            //rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0,0,0),3);

        }
    }
    return myPoint;

}

//this function is going to draw the circle on the tip, it accepts values of color and points of center for circle
void drawOnTip (vector<vector<int>> newPoints, int defColors){

    if (defColors == 0){
        for(int i = 0; i<newPoints.size(); i++){
                //here radius of the circle will keep increasing as we go near the tip and color will brighten as well
                circle(img,Point(newPoints[i][0],newPoints[i][1]),1+i,Scalar(0,125+i*15,0),FILLED);
        }
    }
    else
        for(int i = 0; i<newPoints.size(); i++){
                //here radius of the circle will keep increasing as we go near the tip and color will brighten as well
                circle(img,Point(newPoints[i][0],newPoints[i][1]),1+i,Scalar(125+i*15,0,0),FILLED);
        }
}

void findColors(Mat &img){

    Mat imgHSV, mask;

    cvtColor(img, imgHSV, COLOR_BGR2HSV);

    for(int i = 0; i<myColors.size(); i++){

        Scalar lower(myColors[i][0], myColors[i][2], myColors[i][4]); //format Hmin, Smin, Vmin
        Scalar upper(myColors[i][1],myColors[i][3], myColors[i][5]); //format Hmax, Smax, Vmax
        //now here we are creating mask
        inRange(imgHSV,lower,upper,mask); 
        //imshow(to_string(i), mask);
        Point myPoint = getContours(mask);

        if(myPoint.x!=0 && myPoint.y!=0 && i==0){
            newPointsColor1.push_back({myPoint.x, myPoint.y, i}); //global variable
            if (newPointsColor1.size()>12){ //we will have 12 points on tip tails, removing the old points, latest 12 is only allowed
                newPointsColor1.erase(newPointsColor1.begin());
            }
        }

        if(myPoint.x!=0 && myPoint.y!=0 && i==1){
            newPointsColor2.push_back({myPoint.x, myPoint.y, i}); //global variable
            if (newPointsColor2.size()>12){ //we will have 12 points on tip tails, removing the old points, latest 12 is only allowed
                newPointsColor2.erase(newPointsColor2.begin());
            }
        }
    }
    drawOnTip(newPointsColor1,0); //0 for Green color
    drawOnTip(newPointsColor2,1); //1 for Blue color
}


int main(){

    VideoCapture vdo(0); 
    
    while(true){
    
        vdo.read(img); 

        //funciton to find the colors
        findColors(img);

        imshow("Main Frame", img); 
        waitKey(1); 
    }

}
