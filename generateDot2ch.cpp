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
uchar GetColor(IplImage *srcImage, int x, int y){
	return ((uchar*)(srcImage->imageData + srcImage->widthStep * y))[x];
}
/* 出力画像に色をセットする関数 */
void setColor(IplImage *image, int x, int y, int num, int color){
	for (int i=0; i<num; i++) {
		for (int j=0; j<num; j++) {
			if (x+j >= image->width || y+i >= image->height) {
				break;
			}
			cvSetReal2D(image,y+i,x+j,color);
		}
	}
}

int main(int argc){
	cout << "Content-type: text/html\n\n";
	/*画像読み込み*/
	IplImage *srcImage = 0;	//入力画像
	IplImage *rsImage = 0;	//リサイズ後画像
	IplImage *dstImage = 0;	//出力画像
	uchar c=0;
	srcImage = cvLoadImage("images/getImage.png",CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_GRAYSCALE);
	if(srcImage == 0)
		return -1;
	/*升目であまりが出たときややこしいのでリサイズ．画像サイズによってリサイズ後のサイズも変更するよう改善します*/
	rsImage = cvCreateImage(cvSize(120,(srcImage->height*120)/srcImage->width),IPL_DEPTH_8U,1);
	cvResize(srcImage,rsImage,CV_INTER_CUBIC);
	dstImage = cvCreateImage(cvGetSize(rsImage),IPL_DEPTH_8U,1);
	int size = 30;	//升目数
	int sizeNum = rsImage->height/size;	//1マスあたりのピクセル数
	
	int x=0,y=0;	//座標
	int xycolor[120][120]={0};
	int color[2]={0};	//各座標の{0%,50%,100%}数
	while (y<rsImage->height) {
		x=0;	//次のy座標にいくたびに先頭のx座標に戻る
		while (x <rsImage->width){
			/*升目内の色を取得*/
			c = GetColor(rsImage, x, y);
			/*値によって白黒に振り分け*/
			if (c<128){
				color[0]++;
				xycolor[x][y] = 0;
			}
			else{
				color[1]++;
				xycolor[x][y] = 255;
			}
			x++;
		}
		y++;
	}
	x=0;y=0;
	int minor = 100,mager=100;
	if(color[0] >color[1]*4){
		minor = 255,mager = 0;
	}else if (color[1] > color[0]*4) {
		minor = 0,mager = 255;
	}
	if (minor != 100) {
	while (y<rsImage->height) {
		x=0;	//次のy座標にいくたびに先頭のx座標に戻る
		while (x <rsImage->width){
			c = mager;
			for (int i=0; i<sizeNum; i++) {
				for (int j=0; j<sizeNum; j++) {
					if (xycolor[x+j][y+i] == minor) {
						c = minor;
						break;
					}
				}
			}
			//セットした値をマス目内の座標に適用
			setColor(dstImage,x,y,sizeNum,c);
			//マス目の座標分移動
			x+=sizeNum;
		}
		y+=sizeNum;
	}
	}
	if(minor ==100){
		while (y<rsImage->height) {
			x=0;	//次のy座標にいくたびに先頭のx座標に戻る
			while (x <rsImage->width){
				int cnt[2] = {0};
				for (int i=0; i<sizeNum; i++) {
					for (int j=0; j<sizeNum; j++) {
						if (xycolor[x+j][y+i] == 0) {
							cnt[0]++;
						}
						else if (xycolor[x+j][y+i] == 255) {
							cnt[1]++;
						}
					}
				}
				if (cnt[0] > cnt[1]) {
					c=0;
				}else {
					c=255;
				}

				
				//セットした値をマス目内の座標に適用
				setColor(dstImage,x,y,sizeNum,c);
				//マス目の座標分移動
				x+=sizeNum;
			}
			y+=sizeNum;
		}
	}
	
	cvSaveImage("images/dstImage2ch.png",dstImage);
	printf("<div><img src=\"images/dstImage2ch.png\"></div>");
	printf("<div>この絵からイラストロジックを作成しますか？修正する場合は画像をダウンロードし，修正したファイルをアップロードしてください．</div>");
	printf("背景色：<form action='./generateIL2ch.cgi' medthod='get'><select name='color' size='2'><option value='white' selected>白</option><option value='black'>黒</option></select>");
	printf("<input type='submit' value='作成する'></form>　<a href=\"./revise.php\">作成しない</a><br>");
	printf("修正画像アップロード:");
	printf("<form method=\"post\" action=\"./revisedUp.php\" enctype=\"multipart/form-data\"><input type=\"file\" name=\"revised\"><input type=\"submit\" value=\"アップロード\"></form>");

	return 0;
}
