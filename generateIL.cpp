/**********************************************
 
 北陸先端科学技術大学院大学　知識科学研究科
 博士前期課程　1年　菊川真理子
 
 ドット絵からイラストロジックを作成し，表示するプログラムです
 
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

/*次のマスの色を取得する関数nextColor(image,x座標,y座標,マス内のピクセル数,行=true:列=false)*/
int nextColor(IplImage * srcImage, int x, int y, int sizeNum,bool ver){
	uchar r=0,g=0,b=0;
	int nc = 0;
	/*もし行のヒントだったら次のマスは下のマス*/
	if (ver == true) {
		/*次のマスがある場合は色取得*/
		if(srcImage->height - y > sizeNum){
			r = GetColorR(srcImage, x, y+sizeNum);
			g = GetColorG(srcImage, x, y+sizeNum);
			b = GetColorB(srcImage, x, y+sizeNum);
			/*0だと桁上りしないため，一時的に1に変更*/
			if(r==0) r=1; if(g==0) g=1; if(b==0) b=1;
			/*RGB3桁ずつ，計9桁の整数で色を保存しておく*/
			nc =  r*1000000+g*1000+b;
		}
		/*もし最後のマスだったら0にする*/		
		else {
			nc =  0;
		}
	}
	/*もし列のヒントだったら次のマスは右のマス*/
	else{
		if(srcImage->width - x > sizeNum){
			r = GetColorR(srcImage, x+sizeNum, y);
			g = GetColorG(srcImage, x+sizeNum, y);
			b = GetColorB(srcImage, x+sizeNum, y);
			if(r==0) r=1; if(g==0) g=1; if(b==0) b=1;
			nc =  r*1000000+g*1000+b;
		}else {
			nc =  0;
		}
	}
	return nc;
}

/*各行・列のヒントを取得*/
void getHints(int hints[30][30][2],IplImage *srcImage,int size,int sizeNum,bool ver){
	uchar r=0,g=0,b=0;
	/*********************
	 nowSets=現在の行or列
	 now=現在のマス
	 lenSets=行or列の総数
	 len=捜索中の行or列内のマス数
	 cellSets=現在の行or列
	 cell=行or列中何番目のヒントであるか
	 イメージとしてはnowSets番目の中のnow番目のマスという感じです
	 *********************/	 
	int x=0,y=0,nowSets=0,now=0,lenSets=0,len=0,cellSets=0,cell=0;
	/*初期値として本来当てはまらない値を設定*/
	for (int i=0; i<30; i++) {
		for (int j=0; j<30; j++) {
			for (int k=0; k<2; k++) {
				hints[i][j][k] = size+1;
			}
		}
	}
	/*行の場合行数をlenSetsに，行中のマス数をlenに．
	 列の場合は列数をlenSetsに，列中のマス数をlenに*/
	if(ver == true){
		lenSets = srcImage->width;
		len = srcImage->height;
	}else {
		lenSets = srcImage->height;
		len = srcImage->width;
	}
	/*行の場合は現在の行のx座標をxに
	 列の場合は現在の列のy座標をyに代入*/
	if(ver==true) x = nowSets; else y = nowSets;
	/*1行or列目からマスの色と数を取得．最後の列or行までループ*/
	while (nowSets<lenSets) {
		/*新しい行or列では捜索するマス・ヒント番号を先頭に戻す*/
		now=0;cell=0;
		/*行の場合は現在のマスのy座標をyに
		 列の場合は現在のマスのx座標をxに代入*/
		if(ver==true) y = now; else x = now;
		/*行or列の末尾までマスを進めつつループ*/
		while (now <len){
			hints[cellSets][cell][0] = 1;	//この時点でcellSet行or列目のcell番目のヒントは(不明)色が1マス
			/*色取得*/
			r = GetColorR(srcImage, x, y);
			g = GetColorG(srcImage, x, y);
			b = GetColorB(srcImage, x, y);
			/*0だと桁上りしないため，一時的に1に変更*/
			if(r==0) r=1; if(g==0) g=1; if(b==0) b=1;
			/*RGB3桁ずつ，計9桁の整数で色を保存しておく*/
			hints[cellSets][cell][1] = r*1000000+g*1000+b;
			/*次のマスが同色である場合以下の処理．マスを進めていって違う色になるまでループ
			 ここでのループによってcellSets行or列目のcell番目のヒントはhints[cellSets][cell][1]の色が
			 hints[cellSets][cell][0]続いているという意味のものになる*/
			while (hints[cellSets][cell][1] == nextColor(srcImage,x,y,sizeNum,ver)) {
				now+=sizeNum;	//現在座標+マス目ピクセル数で次のマスの座標へと移る
				if(ver==true) y = now; else x = now;
				hints[cellSets][cell][0]++;	//ヒントとして表示される数字を+1
			}
			if (hints[cellSets][cell][1] != 255255255) {	//白以外の場合次のヒントへ
				cell++;
			}else if(hints[cellSets][cell][0] == size){
				hints[cellSets][cell][0] = 0;	//白のみだったら0と表示
			}else {
				hints[cellSets][cell][0] = size+1;	//白マスは空にする(ヒントを進めずに上書き)
			}
			/*次のマス座標の取得，代入*/
			now+=sizeNum;
			if(ver==true) y = now; else x = now;
		}
		/*次の行or列座標の取得，代入*/
		cellSets++;
		nowSets+=sizeNum;
		if(ver==true) x = nowSets; else y = nowSets;
	}
}
/*列・行ごとの最大ヒント数を返す*/
int getMaxHint(int hints[30][30][2],int size){
	int Max=0,tmp=0;
	for (int i=0; i<30; i++) {
		/*各行・列ごとのヒント数*/
		for (int j=0; j<30; j++) {
			/*ヒントが空白になったら，そこまでのヒント数を保存*/
			if(hints[i][j][0] == size+1){
				tmp = j;
				break;
			}
		}
		/*最大ヒント数よりヒント数が大きければ，代入*/
		if(tmp > Max)
			Max = tmp;
	}
	return Max;
}
/*ヒント表示のために数字を後ろに詰め，余った前の部分に空白を挿入する*/
void arrayPush(int hints[30][30][2],int max,int size){
	for (int i=0; i<30; i++) {	//行・列の移動
		int numCell = 0;	//数字を見つけたマス
		for (int j=max-1; j>=0; j--) {	//下から数える
			if (hints[i][j][0] != 31) {	//数字があったら
				numCell = j;	//数字を見つけたマスを保存
				for (int k=numCell; k>=0; k--) {
					hints[i][max-1-(numCell-k)][0] = hints[i][k][0];	//数字を下に詰める
					hints[i][max-1-(numCell-k)][1] = hints[i][k][1];	//色も入れ替え
				}
				break;
			}
		}
		/*余った部分を空白にする*/
		for (int j=0; j<max-1-numCell; j++) {
			hints[i][j][0] = size+1;
		}
	}
}

int main(int argc, char **argv){
	cout << "Content-type: text/html\n\n";
	/*画像ロード*/
	IplImage *srcImage = 0;
	srcImage = cvLoadImage("images/dstImage.png",CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	if(srcImage == 0)
		return -1;
	int size = 30;	//マス数(今回は30で固定)
	int sizeNum = srcImage->height/size;	//1マス数内のピクセル数
	int hintsVer[30][30][2],hintsHol[30][30][2];	//行ヒントセットと，列ヒントセット[何行or列目][何番目の数字か][0:数字,1:色]
	int maxVer=0, maxHol=0;	//行最大ヒント数と列最大ヒント数
	getHints(hintsVer,srcImage,size,sizeNum,true);//縦ヒント取得
	getHints(hintsHol,srcImage,size,sizeNum,false);	//横ヒント取得
	maxVer = getMaxHint(hintsVer,size); maxHol = getMaxHint(hintsHol,size);	//行・列最大ヒント数取得
	arrayPush(hintsVer,maxVer,size); arrayPush(hintsHol,maxHol,size);	//ヒントを後ろに詰める
	int cellSize = 15;	//出力結果におけるマスの大きさ
	/*ヒントを表示する部分の長さ．hintVerLen:行ヒント表示に必要な高さ hintHolLen列ヒント表示に必要な幅*/
	int hintVerLen = maxVer* cellSize, hintHolLen= maxHol* cellSize;
	int borderWidth = 1;	//通常の線の太さ
	int borderMWidth = 2;	//5マス区切り線の太さ
	int markCnt = size/5 -1;	//5マス区切り線の数
	/*borderPlus:解答ゾーンにおける境界線の太さを全て足した値*/
	int borderPlus = ((size-markCnt-1)*borderWidth)+(markCnt*borderMWidth);
	/*border(Ver|Hol)Plus:ヒント間の境界線の太さ．今回は0*/
	int borderVerPlus = 0;
	int borderHolPlus = 0;
	/***********************************************
	 問題の描画はdivタグを利用
	 Safariで印刷しようとすると綺麗に印刷用PDFが生成されます
	 5マスごとに目印として線を1ｐｘ太くしています．
	 ***********************************************/
	printf("<div style=\"height:%dpx\">",hintVerLen+borderVerPlus);
	/*左上の何も無いスペース*/
	printf("<div style=\"float:left;height:%dpx;width:%dpx;border-bottom:3px solid black; border-right:3px solid black;\"></div>",hintVerLen+borderVerPlus,hintHolLen+borderHolPlus);
	/*縦ヒント*/
	printf("<div style=\"float:left; height:%dpx;width:%dpx; border-width:0px 3px 3px 0px; border-style:solid;\">",hintVerLen+borderVerPlus,size* cellSize+borderPlus);
		for (int i=0; i<maxVer; i++) {
			printf("<div style=\"float:left;height:%dpx; width:%dpx;\"",cellSize,size* cellSize+borderPlus);
			for (int j=0; j<size; j++) {
				int r = hintsVer[j][i][1]/1000000;
				int g = (hintsVer[j][i][1]-r*1000000)/1000;
				int b = hintsVer[j][i][1]-(r*1000000+g*1000);
				if(r==1) r=0; if(g==1) g=0; if(b==1) b=0;
				if(r==255 && g == 255 && b == 255){ r=0; g=0; b=0;}
				if (j== size-1) {
					borderWidth = 0;
				} else if (j %5 == 4){
					borderWidth = 2;
				}
				else {
					borderWidth = 1;
				}
				if (hintsVer[j][i][0] == size+1) {
					printf("<div style=\"float:left;height:%dpx; width:%dpx;border-right:%dpx dashed black;font-size:small;\"></div>",cellSize,cellSize,borderWidth);					
				}else{
					printf("<div style=\"float:left;height:%dpx; width:%dpx;border-right:%dpx dashed black; color:rgb(%d,%d,%d);font-size:small;\">%d</div>",cellSize,cellSize,borderWidth,r,g,b,hintsVer[j][i][0]);
				}
			}
			printf("</div>");
		}
	printf("</div>");
	printf("</div>");
	/*横ヒント*/
	borderWidth = 1;
	printf("<div style=\"clear:both;float:left; height:%dpx;width:%dpx; border-width:0px 3px 3px 0px; border-style:solid;\">",size* cellSize+borderPlus,hintHolLen+borderHolPlus);
	for (int i=0; i<size; i++) {
		if (i== size-1) {
			borderWidth = 0;
		}else if (i% 5== 4) {
			borderWidth = 2;
		}else {
			borderWidth = 1;
		}
		printf("<div style=\"float:left;height:%dpx; width:%dpx;\"",cellSize+borderWidth,hintHolLen+borderHolPlus);
		for (int j=0; j<maxHol; j++) {
			int r = hintsHol[i][j][1]/1000000;
			int g = (hintsHol[i][j][1]-r*1000000)/1000;
			int b = hintsHol[i][j][1]-(r*1000000+g*1000);
			if(r==1) r=0; if(g==1) g=0; if(b==1) b=0;
			if(r==255 && g == 255 && b == 255){ r=0; g=0; b=0;}
			if (hintsHol[i][j][0] == size+1) {
				printf("<div style=\"float:left;height:%dpx; width:%dpx;border-bottom:%dpx dashed black;font-size:small; \"></div>",cellSize,cellSize,borderWidth);				
			}else{
				printf("<div style=\"float:left;height:%dpx; width:%dpx;border-bottom:%dpx dashed black; color:rgb(%d,%d,%d);font-size:small;\">%d</div>",cellSize,cellSize,borderWidth,r,g,b,hintsHol[i][j][0]);
			}
		}
		printf("</div>");
	}
	printf("</div>");
	
	/*解答ゾーン*/
	borderWidth=1;
	printf("<div style=\"float:left; height:%dpx;width:%dpx; border-width:0px 3px 3px 0px; border-style:solid;\">",size* cellSize+borderPlus,size*cellSize+borderPlus);
	for (int i=0; i<size; i++) {
		if (i%5== 4) {
			printf("<div style=\"float:left;height:%dpx; width:%dpx;\"",cellSize+borderMWidth,size* cellSize+borderPlus);
		}else{
			printf("<div style=\"float:left;height:%dpx; width:%dpx;\"",cellSize+borderWidth,size* cellSize+borderPlus);
		}
		for (int j=0; j<size; j++) {
			if( i==size-1 && j== size -1){
				printf("<div style=\"float:left;height:%dpx; width:%dpx;border-width:0px 0px 0px 0px;border-style:solid;\"></div>",cellSize,cellSize);
			}else if (i == size -1) {
				if (j%5== 4) {
					printf("<div style=\"float:left;height:%dpx; width:%dpx;border-width:0px %dpx 0px 0px;border-style:solid;\"></div>",cellSize,cellSize,borderMWidth);
				}else {
					printf("<div style=\"float:left;height:%dpx; width:%dpx;border-width:0px %dpx 0px 0px;border-style:solid;\"></div>",cellSize,cellSize,borderWidth);
				}
			}else if (j== size- 1) {
				if (i%5== 4) {
					printf("<div style=\"float:left;height:%dpx; width:%dpx;border-width:0px 0px %dpx 0px;border-style:solid;\"></div>",cellSize,cellSize,borderMWidth);
				}else {
					printf("<div style=\"float:left;height:%dpx; width:%dpx;border-width:0px 0px %dpx 0px;border-style:solid;\"></div>",cellSize,cellSize,borderWidth);
				}
			}else if (i%5==4 && j%5== 4) {
				printf("<div style=\"float:left;height:%dpx; width:%dpx;border-width:0px %dpx %dpx 0px;border-style:solid;\"></div>",cellSize,cellSize,borderMWidth,borderMWidth);
			}else if(i%5==4){
				printf("<div style=\"float:left;height:%dpx; width:%dpx;border-width:0px %dpx %dpx 0px;border-style:solid;\"></div>",cellSize,cellSize,borderWidth,borderMWidth);
			}else if(j%5==4){
				printf("<div style=\"float:left;height:%dpx; width:%dpx;border-width:0px %dpx %dpx 0px;border-style:solid;\"></div>",cellSize,cellSize,borderMWidth,borderWidth);
			}else{
				printf("<div style=\"float:left;height:%dpx; width:%dpx;border-width:0px %dpx %dpx 0px;border-style:solid;\"></div>",cellSize,cellSize,borderWidth,borderWidth);
			}
		}
		printf("</div>");
	}
	printf("</div>");
	return 0;
}
