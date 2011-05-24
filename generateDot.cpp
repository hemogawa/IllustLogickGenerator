/**********************************************
 
 北陸先端科学技術大学院大学　知識科学研究科
 博士前期課程　1年　菊川真理子
 
 イラストからドット絵を生成するプログラムです．
 
 **********************************************/
#include<iostream>
#include<cv.h>
#include<stdio.h>
#include<cxcore.h>
#include<highgui.h>
#include<math.h>
#include<string.h>
#include<sstream>

using namespace std;

/*色を取得する関数*/
uchar GetColorR(IplImage *srcImage, int x, int y){
	return ((uchar*)(srcImage->imageData + srcImage->widthStep * y))[x * 3 + 2];
}
uchar GetColorG(IplImage *srcImage, int x, int y){
	return ((uchar*)(srcImage->imageData + srcImage->widthStep * y))[x * 3 + 1];
}
uchar GetColorB(IplImage *srcImage, int x, int y){
	return ((uchar*)(srcImage->imageData + srcImage->widthStep * y))[x * 3];
}

int main(int argc){
	cout << "Content-type: text/html\n\n";
	/*画像読み込み*/
	IplImage *srcImage = 0;	//入力画像
	IplImage *rsImage = 0;	//リサイズ後画像
	IplImage *dstImage = 0;	//出力画像
	uchar r=0, g=0, b=0, c=0;
	srcImage = cvLoadImage("images/getImage.png",CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	if(srcImage == 0)
		return -1;
	/*升目であまりが出たときややこしいのでリサイズ．画像サイズによってリサイズ後のサイズも変更するよう改善します*/
	rsImage = cvCreateImage(cvSize(120,(srcImage->height*120)/srcImage->width),IPL_DEPTH_8U,3);
	cvResize(srcImage,rsImage,CV_INTER_CUBIC);
	dstImage = cvCreateImage(cvGetSize(rsImage),IPL_DEPTH_8U,3);
	int size = 30;	//升目数
	int sizeNum = rsImage->height/size;	//1マスあたりのピクセル数
	
	int x=0,y=0;	//座標
	while (y<rsImage->height) {
		x=0;	//次のy座標にいくたびに先頭のx座標に戻る
		while (x <rsImage->width){
			int color[3][3]={0};	//各座標の{{r,g,b}{0%,50%,100%}}数
			/*升目内の色を取得*/
			for (int i=0; i<sizeNum; i++) {
				for (int j=0; j<sizeNum; j++) {
					/*r,g,bの順に値を取得するようにループ*/
					for (int k=0; k<3; k++) {
						if(k==0)
							c = GetColorR(rsImage, x+j, y+i);
						else if(k==1)
							c= GetColorG(rsImage,x+j,y+i);
						else
							c = GetColorB(rsImage, x+j, y+i);
						/*値によって0%,50%,100%に振り分け*/
						if (c<85)
							color[k][0]++;
						else if(c<170)
							color[k][1]++;
						else
							color[k][2]++;
					}
				}
			}
			int maxCol[3]={0};
			/*r,g,b各値で最も多い値(0%か50%か100%)を判定*/
			for (int i=0; i<3; i++) {
				int tmpMax = 0;
				for (int j=0; j<3; j++) {
					if(color[i][j]>tmpMax){
						tmpMax = color[i][j];
						maxCol[i] = j;
					}					
				}
			}
			/*rgbごとに最も多い値に合わせて値をセット*/
			for (int i=0; i<3; i++) {
				if(maxCol[i] == 0){
					if (i==0) {
						r=0;
					}else if (i==1) {
						g=0;
					}else{
						b=0;
					}
				}else if (maxCol[i] == 1) {
					if (i==0) {
						r=127.5;
					}else if(i==1) {
						g=127.5;
					}else{
						b=127.5;
					}
				}else {
					if (i==0) {
						r=255;
					}else if (i==1) {
						g=255;
					}else {
						b=255;
					}
				}
			}
			/*セットした値をマス目内の座標に適用*/
			for (int i=0; i<sizeNum; i++) {
				for (int j=0; j<sizeNum; j++) {
					if (x+j >= dstImage->width || y+i >= dstImage->height) {
						break;
					}
					cvSet2D(dstImage,y+i,x+j,CV_RGB(r,g,b));
				}
			}
			/*マス目の座標分移動*/
			x+=sizeNum;
		}
		y+=sizeNum;
	}
	cvSaveImage("images/dstImage.png",dstImage);
	printf("<div><img src=\"images/dstImage.png\"></div>");
	printf("<div>この絵からイラストロジックを作成しますか？修正する場合は画像をダウンロードし，修正したファイルをアップロードしてください．</div>");
	printf("<div><a href=\"./generateIL.cgi\">作成する</a>　<a href=\"./revise.php\">作成しない</a></div>");
	printf("修正画像アップロード:");
	printf("<form method=\"post\" action=\"./revisedUp.php\" enctype=\"multipart/form-data\"><input type=\"file\" name=\"revised\"><input type=\"submit\" value=\"アップロード\"></form>");

	return 0;
}
