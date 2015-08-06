// Find primes below one million ending in '9999'.
function primeTest() {
    var res = [];

    /*print('Have native helper: ' + (primeCheckHelper !== primeCheckEcmascript));
    for (var i = 1; i < 1000000; i++) {
        if (primeCheck(i) && (i % 10000) == 9999) { res.push(i); }
    } 
    print(res.join(' '));*/

    var canvas = new Canvas(); 
    var ctx = canvas.getContext("2d");
//	ctx.scale(2,3);
//	ctx.rotate(20);
//	ctx.translate(50,50);
//	ctx.transform(1,0.5,-0.5,1,30,10);
	ctx.save();
	ctx.lineWidth = 5.0;
	ctx.fillStyle = "#00FF00";
	ctx.moveTo(20,20);
	ctx.lineTo(50,50);
	ctx.lineTo(20,50);
	ctx.lineTo(80,30);
//	ctx.rect(50,50,50,50);
//	xsColor black = {255,0,0,0};
//	ctx.strokeColor = black;
	ctx.stroke();
//	xsColor red = {255,255,0,0};
//	ctx.fillColor = red;
	ctx.fill();
	ctx.beginPath();
	ctx.lineTo(120,120);
	ctx.lineTo(150,150);
	ctx.restore();
	ctx.closePath();
	ctx.stroke();
	ctx.fillRect(130,20,20,20);
	ctx.strokeRect(160,20,20,20);
	ctx.clearRect(20,20,20,20);
	ctx.arc(20,100,20,0, 3.14, false);
	ctx.bezierCurveTo(100, 100, 130, 80, 170, 100);
	ctx.stroke();
	ctx.fill();
	//ctx.arc(20,100,20,0,90,true);
	//xsImage image = {XS_IMGTYPE_UNKNOWN, XS_AFD_FILENAME, L"c:\\icon_bt.bmp", 0, 0, 0};
	//ctx.drawImage(image,100,100);
	//ctx.drawImage(&image,20,20,20,20,20,20,20,20);
	ctx.fillText("AAAAA",50,50,4);
	ctx.strokeText("def",100,200,50);

}

//7个基本形状，及变形后的坐标
var shap1=[[0,0,1,0,1,1,1,2],[0,1,1,1,2,1,2,0],[0,0,0,1,0,2,1,2],[0,0,1,0,2,0,0,1]];
var shap2=[[0,0,0,1,0,2,1,0],[0,0,1,0,2,0,2,1],[0,2,1,0,1,1,1,2],[0,0,0,1,1,1,2,1]];
var shap3=[[0,0,1,0,1,1,2,1],[1,0,1,1,0,1,0,2],[0,0,1,0,1,1,2,1],[1,0,1,1,0,1,0,2]]
var shap4=[[0,1,1,0,1,1,2,0],[0,0,0,1,1,1,1,2],[0,1,1,0,1,1,2,0],[0,0,0,1,1,1,1,2]];
var shap5=[[0,0,1,0,0,1,1,1],[0,0,1,0,0,1,1,1],[0,0,1,0,0,1,1,1],[0,0,1,0,0,1,1,1]];
var shap6=[[0,1,1,1,2,1,1,0],[0,0,0,1,0,2,1,1],[0,0,1,0,2,0,1,1],[1,0,1,1,0,1,1,2]];
var shap7=[[0,0,1,0,2,0,3,0],[0,0,0,1,0,2,0,3],[0,0,1,0,2,0,3,0],[0,0,0,1,0,2,0,3]];
var shaps=[shap1,shap2,shap3,shap4,shap5,shap6,shap7];

//单位长度
var unitLen=10;
//画布上下文
var ctx;
//定时
var tid;
//画布区域对应的数组
var resultArray=new Array(40);
function init()
{
    var canvas = new Canvas();
//获取画布上下文
ctx=canvas.getContext('2d');
//增加按键事件
document.addEventListener('keydown',moveShape,false);

//初始化画布区域数组
for(var i=0;i<40;i++)
{
	var row=new Array();
	for(var j=0;j<20;j++)
	{
	    row[j]=0;
	}
	resultArray[i]=row;
}
//
startRun=true;
DrawLine();
topTrue=true;
tid=setInterval("DrawTetris();",300);

DrawTetris();
}

//每个形状在画布上相对于初始位置的坐标偏移量
var rectX=0;
var rectY=0;

//形状变形
var rotate=0;
//当前形状或变形的初始坐标
var t;
//当前形状
var shape;

var startRun=true;


var shapeHeight=0;//当前形状的高度，四个方块的Y坐标的最大差
//用于产生随机形状
var randmShape=1;

//当前形状每个方块的坐标
var shapeXY=new Array(4);

//根据坐标绘制形状
function Draw(){
var i=0;
var tempY=0;
shapeXY=new Array(4);
for(i=0;i<4;i++){
	DrawRect((t[i*2]+rectX) *unitLen,(t[i*2+1]+rectY)*unitLen);
	var row=new Array(2);
	row[0]=(t[i*2+1]+rectY);
	row[1]=t[i*2]+rectX;
	shapeXY[i]=row;
	if(topTrue==true)
	{
	    if(tempY<(t[i*2+1]+rectY))
	    {
	        tempY=t[i*2+1]+rectY;
	        shapeHeight=tempY+1;
	    }

	}
	else
	{
	    //tempY=rectY;
	}
}
rectY+=1;

}

//根据捕获的按键，判断具体的按键
function getDirection(event){
var keyCode = event.which || event.keyCode;
switch(keyCode){
	case 1:
	case 38:
	case 269: //up
	    return 'up';
	    break;
	case 2:
	case 40:
	case 270:
	    return 'down';
	    break;
	case 3:
	case 37:
	case 271:
	    return 'left';
	    break;
	case 4:
	case 39:
	case 272:
	    return 'right';
	    break;
	case 339: //exit
	case 240: //back
	    return 'back';
	    break;
}
}
//根据按键确定执行的操作
function moveShape(event){
if(getDirection(event)=='right')
{
	for(var i=0;i<4;i++){
	    if(t[2*i]+rectX+1>=20 || resultArray[2*i+rectX+1]==1 ){
	        return;
	    }
	}
	rectX+=1;
}
if(getDirection(event)=='left')
{
	for(var i=0;i<4;i++)
	{
	    if(t[2*i]+rectX-1<0 || resultArray[2*i+rectX-1]==1){
	        return;
	    }
	}
	rectX-=1;

}
if(getDirection(event)=='up'){
	var mleft=0;
	for(var i=0;i<4;i++){
	    if(t[i*2]+rectX>mleft){
	        mleft=rectX;
	    }
	}
	if(rotate==3){
	    rotate=0;
	}
	else{
	    rotate+=1;
	}
	t=shape[rotate];
	for(var i=0;i<4;i++){

	    //t[2*i]=t[2*i]+mleft;
	    rectX=mleft;
	}

}

if(getDirection(event)=='down'){
	clearInterval(tid);
	tid=setInterval("DrawTetris();",100);
}
}
//定时执行的方法
function DrawTetris(){
if(CheckBottom()==true) return;
if(startRun==false){
	ctx.clearRect(shapeXY[0][1]*unitLen-1,shapeXY[0][0]*unitLen-1,unitLen+2,unitLen+2);
	ctx.clearRect(shapeXY[1][1]*unitLen-1,shapeXY[1][0]*unitLen-1,unitLen+2,unitLen+2);
	ctx.clearRect(shapeXY[2][1]*unitLen-1,shapeXY[2][0]*unitLen-1,unitLen+2,unitLen+2);
	ctx.clearRect(shapeXY[3][1]*unitLen-1,shapeXY[3][0]*unitLen-1,unitLen+2,unitLen+2);
}
startRun=false;
DrawLine();
var sp=randmShape;
shape=shaps[sp-1];
t=shape[rotate];
Draw();

}

var topTrue=false;
//检查当前形状是否到了画布底部
function CheckBottom()
{
if(topTrue==true){
	startRun=true;
	topTrue=false;
	rectX=9;
	rectY=0;
	randmShape=Math.floor(Math.random()*7+1);

	return true;
}

if(rectY+shapeHeight-1>=40 || rectY==0)
{

	if(rectY==0)
	{
	    return false
	}

	CurrentShapeOnBottom();
	return true;
}
else
{
	//形状中的四个方块有一个到了底部，就不能再向下移动
	if(shapeXY[0][0]==39 || shapeXY[1][0]==39 || shapeXY[2][0]==39 || shapeXY[3][0]==39)
	{
	    CurrentShapeOnBottom();
	    return true;
	}
	//形状中的每个方块所在行的下一行，如果已经存在方块，不能再向下移动
	if((resultArray[shapeXY[0][0]+1][shapeXY[0][1]]+resultArray[shapeXY[1][0]+1][shapeXY[1][1]]
	        +resultArray[shapeXY[2][0]+1][shapeXY[2][1]]+resultArray[shapeXY[3][0]+1][shapeXY[3][1]]
	       >=1) )
	{
	    CurrentShapeOnBottom();
	    return true;
	}
	topTrue=false;
	return false;
}
}

//当前形状到达画布底部后进行的操作
function CurrentShapeOnBottom(){

resultArray[shapeXY[0][0]][shapeXY[0][1]]=1;
resultArray[shapeXY[1][0]][shapeXY[1][1]]=1;
resultArray[shapeXY[2][0]][shapeXY[2][1]]=1;
resultArray[shapeXY[3][0]][shapeXY[3][1]]=1;

if(ClearRow()==false){
	return;
}
rectY=0;
rectX=9;
randmShape=Math.floor(Math.random()*7+1)
startRun=true;
topTrue=true;
clearInterval(tid);
tid=setInterval("DrawTetris();",300);
}
//计分
var vpoint=0;
//清除满格行,并计分
function ClearRow(){
var row=new Array();
var spaceRow=new Array();
var spaceRows=new Array();
for(var i=0;i<20;i++){
	spaceRow[i]=0;
}
row[0]=shapeXY[0][0];
for(var i=1;i<4;i++){
	for(var j=0;j<row.length;j++){
	    if(row[j]!=shapeXY[i][0]){
	        if(row[j]<shapeXY[i][0]){
	            row.push(shapeXY[i][0]);

	        }
	        else {
	            row.unshift(shapeXY[i][0]);
	        }
	        break;

	    }
	}
}

var isNeedRedraw=false;
for(var i=0;i<row.length;i++){

	var rowState=0;
	for(var j=0;j<20;j++){
	    rowState+=resultArray[row[i]][j];
	}



	if(rowState==20){
	    resultArray.splice(row[i],1);
	    resultArray.unshift(spaceRow);
	    vpoint+=10;
	    //document.getElementById("txtPoint").value=vpoint;
	    isNeedRedraw=true;
	}
}

if(isNeedRedraw==true){
	ctx.clearRect(0,0,200,400);
	RedrawCanvas();

}
else
{
	if(shapeXY[0][0]==0 || shapeXY[1][0]==0 || shapeXY[2][0]==0 || shapeXY[3][0]==0){
	    //document.getElementById("idState").innerText="Game Over";
	    clearInterval(tid);
	    return false;
	}
}
return true;
}

//清除满格行后，重新绘制画布
function RedrawCanvas()
{
for(var i=0;i<40;i++){
	for(var j=0;j<20;j++){
	    if(resultArray[i][j]==1){
	        DrawRect(j*unitLen,i*unitLen);
	    }
	}
}
}
//绘制形状中的小方格
function DrawRect(x,y){
ctx.fillStyle="#FF0000";
ctx.strokeStyle="#000000";
ctx.lineWidth=1.0;
ctx.fillRect(x,y,10,10);
ctx.strokeRect(x,y,10,10);
}


function DrawLine()
{
/*
var i=1;
for(i=1;i<40;i++)
{
	ctx.beginPath();
	ctx.strokeStyle="black";
	ctx.lineWidth=1;
	ctx.moveTo(0,i*10);
	ctx.lineTo(200,i*10);
	ctx.stroke();
}
*/
}
