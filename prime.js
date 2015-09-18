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
	ctx.stroke();
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
	ctx.fillStyle = "#00FF00";
	ctx.fillText("AAAAA",10,50,50);
	ctx.strokeText("def",10,100,200);
	//ctx.fillStyle = rgb(255,0,0,0.5);
	//ctx.fill();
	var img = new Image("/home/lewis/git/xs-new/icon_bt.bmp");
	ctx.drawImage(img, 200, 200);
}

//7个基本形状，及变形后的坐标
var shap1=[[0,0,1,0,1,1,1,2],[0,1,1,1,2,1,2,0],[0,0,0,1,0,2,1,2],[0,0,1,0,2,0,0,1]];
var shap2=[[0,0,0,1,0,2,1,0],[0,0,1,0,2,0,2,1],[0,2,1,0,1,1,1,2],[0,0,0,1,1,1,2,1]];
var shap3=[[0,0,1,0,1,1,2,1],[1,0,1,1,0,1,0,2],[0,0,1,0,1,1,2,1],[1,0,1,1,0,1,0,2]];
var shap4=[[0,1,1,0,1,1,2,0],[0,0,0,1,1,1,1,2],[0,1,1,0,1,1,2,0],[0,0,0,1,1,1,1,2]];
var shap5=[[0,0,1,0,0,1,1,1],[0,0,1,0,0,1,1,1],[0,0,1,0,0,1,1,1],[0,0,1,0,0,1,1,1]];
var shap6=[[0,1,1,1,2,1,1,0],[0,0,0,1,0,2,1,1],[0,0,1,0,2,0,1,1],[1,0,1,1,0,1,1,2]];
var shap7=[[0,0,1,0,2,0,3,0],[0,0,0,1,0,2,0,3],[0,0,1,0,2,0,3,0],[0,0,0,1,0,2,0,3]];
var shaps=[shap1,shap2,shap3,shap4,shap5,shap6,shap7];

//单位长度
var unitLen;
//canvas对象
var canvas
//画布上下文
var ctx;
//下落间隔
var fallInterval = 800;; 
//定时
var tid;
//游戏是否结束
var isGameOver = false;
//游戏是否暂停
var isPaused = false;
//每行最大方块数
var maxNum = 10;
//方块最大层数；
var maxHeight;
//画布区域对应的数组
var resultArray;
function init()
{
    canvas = new Canvas();
    unitLen = parseInt(canvas.width/maxNum);
    maxHeight = parseInt(canvas.height/unitLen);
    resultArray=new Array(maxHeight);
	//获取画布上下文
	ctx=canvas.getContext('2d');
	//增加按键事件
	document.addEventListener('keydown',moveShape,false);

	//初始化画布区域数组
	for(var i=0;i<maxHeight;i++)
	{
		var row=new Array();
		for(var j=0;j<maxNum;j++)
		{
			row[j]=0;
		}
		resultArray[i]=row;
	}

	topTrue=false;
	rectX=maxNum/2-1;
	rectY=0;
	shape=shaps[randmShape-1];
	t=shape[rotate];
	Draw();
	ctx.font = "20px sans-serif";
	ctx.fillText(vpoint.toString(),5,10,10);
	tid=setInterval("DrawTetris()",fallInterval);
	DrawTetris();
}

//每个形状在画布上相对于初始位置的坐标偏移量
var rectX=0;
var rectY=0;

//形状变形
var rotate=Math.floor(Math.random()*4);
//当前形状
var shape;

//当前形状或变形的初始坐标
var t;

//var shapeHeight=0;//当前形状的高度，四个方块的Y坐标的最大差
//用于产生随机形状
var randmShape=Math.floor(Math.random()*7+1);
//下一个形状
var nextShape = Math.floor(Math.random()*7+1);
var nextRotate = Math.floor(Math.random()*4);

//当前形状每个方块的坐标
var shapeXY=new Array(4);

//根据坐标绘制形状
function Draw(){
var i=0;
//var tempY=0;
for(i=0;i<4;i++){
	DrawRect((t[i*2]+rectX)*unitLen,(t[i*2+1]+rectY)*unitLen);
	var row=new Array(2);
	row[0]=(t[i*2+1]+rectY);
	row[1]=t[i*2]+rectX;
	shapeXY[i]=row;
/*	if(topTrue==true)
	{
	    if(tempY<(t[i*2+1]+rectY))
	    {
	        tempY=t[i*2+1]+rectY;
	        shapeHeight=tempY+1;
	    }

	}*/
	var next = shaps[nextShape-1][nextRotate];
	DrawLittleRect((next[i*2]+maxNum*2-4)*unitLen/2,(next[i*2+1])*unitLen/2);
}
	ctx.fillText(vpoint.toString(),5,10,10);

}

function clearBlock()
{
	ctx.clearRect(shapeXY[0][1]*unitLen+1,shapeXY[0][0]*unitLen+1,unitLen-2,unitLen-2);
	ctx.clearRect(shapeXY[1][1]*unitLen+1,shapeXY[1][0]*unitLen+1,unitLen-2,unitLen-2);
	ctx.clearRect(shapeXY[2][1]*unitLen+1,shapeXY[2][0]*unitLen+1,unitLen-2,unitLen-2);
	ctx.clearRect(shapeXY[3][1]*unitLen+1,shapeXY[3][0]*unitLen+1,unitLen-2,unitLen-2);
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
	case 13:
		return 'enter';
		break;
	case 339: //exit
	case 240: //back
	    return 'back';
	    break;
}
}
//根据按键确定执行的操作
function moveShape(event){
if(isGameOver)
{
	if(getDirection(event)=='right')
		restart();

	return;
}

if(getDirection(event) != 'enter' && isPaused == true)
{
	return;
}

if(getDirection(event)=='right')
{
	for(var i=0;i<4;i++){
	    if(t[2*i]+rectX+1>=maxNum || t[i*2+1]+rectY>=maxHeight || ((t[i*2+1]+rectY<maxHeight) && (t[2*i]+rectX+1<maxNum) && (resultArray[t[i*2+1]+rectY][t[i*2]+rectX+1] == 1))){
	        return;
	    }
	}
	clearBlock();
	rectX+=1;
	Draw();
}
if(getDirection(event)=='left')
{
	for(var i=0;i<4;i++)
	{
	    if(t[2*i]+rectX-1<0 || t[i*2+1]+rectY>=maxHeight || ((t[2*i+1]+rectY<maxHeight) && (t[i*2]+rectX-1>=0) && (resultArray[t[i*2+1]+rectY][t[i*2]+rectX-1] == 1))){
	        return;
	    }
	}
	clearBlock();
	rectX-=1;
	Draw();
}
if(getDirection(event)=='up'){
	clearBlock();
	var oldRotate = rotate;
	var oldX = rectX;
	if(rotate==3){
	    rotate=0;
	}
	else{
	    rotate+=1;
	}

	var tmp=shape[oldRotate]
	t=shape[rotate];
	
	for(var i=0;i<4;i++){
		if(t[i*2]+rectX+1>maxNum){
			rectX-=1;
		}
	
	}

	for(var j=0; j<4; j++)
	{
		if(t[j*2+1]+rectY>=maxHeight)
		{
			print("I am in");
			rotate = oldRotate;
	       	t=shape[rotate];
	       	rectX = oldX;
	       	if(t[j*2+1]+rectY>=maxHeight)
			{
				rectY-=1;
			}
		}
		else if((resultArray[t[j*2+1]+rectY][t[j*2]+rectX] == 1)
		|| (tmp[j*2+1]+rectY<maxHeight && (resultArray[tmp[j*2+1]+rectY][tmp[j*2]+oldX] == 1)))
		{
			rotate = oldRotate;
		   	t=shape[rotate];
		   	rectX = oldX;
			if(t[j*2+1]+rectY>=maxHeight || resultArray[t[j*2+1]+rectY][t[j*2]+rectX] == 1)
			{
				rectY-=1;
			}
		}
	}
	
	
	Draw();

}

if(getDirection(event)=='down'){
	DrawTetris();
}

if(getDirection(event)=='enter'){
	if(isPaused == false)
	{
		clearInterval(tid);
		isPaused = true;
	}
	else
	{
		tid=setInterval("DrawTetris()",fallInterval);
		isPaused = false;
	}
}
}
//定时执行的方法
function DrawTetris(){
	if(CheckBottom()==true) {
		return;
	}
	clearBlock();
	Draw();
	rectY+=1;
}

var topTrue=false;
//检查当前形状是否到了画布底部
function CheckBottom()
{
	if(topTrue==true){
		if(reachBottom() == true)
		{
			//print(shapeXY[0][0]+" "+shapeXY[0][1]+"// "+shapeXY[1][0]+" "+shapeXY[1][1]+"// "+shapeXY[2][0]+" "+shapeXY[2][1]+"// "+shapeXY[3][0]+" "+shapeXY[3][1]+"// ");
			resultArray[shapeXY[0][0]][shapeXY[0][1]]=1;
			resultArray[shapeXY[1][0]][shapeXY[1][1]]=1;
			resultArray[shapeXY[2][0]][shapeXY[2][1]]=1;
			resultArray[shapeXY[3][0]][shapeXY[3][1]]=1;
			if(ClearRow()==false){
				return true;
			}
		}
		else
		{
			print("not end");
			clearInterval(tid);
			tid=setInterval("DrawTetris()",fallInterval);
			return false;
		}

		randmShape = nextShape;
		rotate = nextRotate;
		shape=shaps[randmShape-1];
		t=shape[rotate];
		topTrue=false;
		rectX=maxNum/2-1;
		rectY=0;
		nextShape=Math.floor(Math.random()*7+1);
		nextRotate=Math.floor(Math.random()*4);
		ctx.clearRect((maxNum*2-4)*unitLen/2,0,4*unitLen/2,4*unitLen/2);
		for(var i=0; i<4; i++)
		{
			DrawRect((t[i*2]+rectX)*unitLen,(t[i*2+1]+rectY)*unitLen);
			var row=new Array(2);
			row[0]=(t[i*2+1]+rectY);
			row[1]=t[i*2]+rectX;
			shapeXY[i]=row;
			var next = shaps[nextShape-1][nextRotate];
			DrawLittleRect((next[i*2]+maxNum*2-4)*unitLen/2,(next[i*2+1])*unitLen/2);
		}
		//print("#########################");
		return true;
	}

	return reachBottom();
}

function reachBottom()
{

	//形状中的四个方块有一个到了底部，就不能再向下移动
	if(shapeXY[0][0]==maxHeight -1 || shapeXY[1][0]==maxHeight -1 || shapeXY[2][0]==maxHeight -1 || shapeXY[3][0]==maxHeight -1)
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

var levelCounter = 0;

function changeSpeed()
{
	if(levelCounter<30)
		return;
	fallInterval=fallInterval-200;
	if(fallInterval<=200)
	{
		fallInterval=200;
	}
	levelCounter=0;
}


//当前形状到达画布底部后进行的操作
function CurrentShapeOnBottom(){
topTrue=true;
clearInterval(tid);
tid=setInterval("DrawTetris()",fallInterval);
}
//计分
var vpoint=0;
//清除满格行,并计分
function ClearRow(){
/*var row=new Array();
var spaceRow=new Array();
for(var i=0;i<maxNum;i++){
	spaceRow[i]=0;
}
row[0]=shapeXY[0][0];
for(var i=1;i<4;i++){
	print("length = " + row.length);
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
	for(var j=0;j<maxNum;j++){
	    rowState+=resultArray[row[i]][j];
	}



	if(rowState==maxNum){
	    resultArray.splice(row[i],1);
	    resultArray.unshift(spaceRow);
	    vpoint+=maxNum;
	    //document.getElementById("txtPoint").value=vpoint;
	    isNeedRedraw=true;
	}
}
*/
var isNeedRedraw=false;
var row = 0;

for(var i=0; i<4; i++)
{
	if(shapeXY[i][0] == row)
		continue;
	row = shapeXY[i][0];
	//print("row = "+row);
	var sum=0;
	for(var j=0; j<maxNum; j++)
	{
		//print("resultArrayNum = "+resultArray[19][j]);
		sum = sum + resultArray[row][j];
	}
	if(sum == maxNum)
	{	
		var spaceRow=new Array(maxNum);

		for(var k=0;k<maxNum;k++){
			spaceRow[k]=0;
		}
		
		resultArray.splice(row,1);
		resultArray.unshift(spaceRow);
		vpoint+=maxNum;
		levelCounter++;
		isNeedRedraw=true;
	}
}

if(isNeedRedraw==true){
	ctx.clearRect(0,0,canvas.width,canvas.height);
	RedrawCanvas();
	changeSpeed();
	ctx.fillText(vpoint.toString(),5,10,10);
}
else
{
	if(rectY == 0){
	    //document.getElementById("idState").innerText="Game Over";
	    isGameOver = true;
	    print("over");
	    clearInterval(tid);
	    ctx.font = "20px sans-serif";
	    ctx.strokeStyle = "#000000";
	    ctx.strokeText("GAME OVER",20,ctx.width/2-30,ctx.height/2,50);
	    return false;
	}
}
return true;
}

//清除满格行后，重新绘制画布
function RedrawCanvas()
{
for(var i=0;i<maxHeight;i++){
	for(var j=0;j<maxNum;j++){
	    if(resultArray[i][j]==1){
	        DrawRect(j*unitLen,i*unitLen);
	    }
	}
}
}

//重新开始游戏
function restart()
{
print("restart");
	isGameOver = false;
	for(var i=0;i<maxHeight;i++)
	{
		for(var j=0;j<maxNum;j++)
		{
			resultArray[i][j]=0;
		}
	}
	
	topTrue=false;
	vpoint = 0;
	levelCounter = 0;
	fallInterval = 800;
	rectX=maxNum/2-1;
	rectY=0;
	shape=shaps[randmShape-1];
	t=shape[rotate];
	nextShape=Math.floor(Math.random()*7+1);
	nextRotate=Math.floor(Math.random()*4);
	ctx.clearRect(0,0,canvas.width,canvas.height);
	ctx.font = "20px sans-serif";
	ctx.fillStyle="#000000";
	ctx.fillText(vpoint.toString(),5,10,10);
	tid=setInterval("DrawTetris()",fallInterval);

}


//绘制形状中的小方格
function DrawRect(x,y){
ctx.fillStyle="#FF0000";
ctx.strokeStyle="#000000";
ctx.lineWidth=1.0;
ctx.fillRect(x+1,y+1,unitLen-2,unitLen-2);
//ctx.strokeRect(x,y,unitLen,unitLen);
}

function DrawLittleRect(x,y){
ctx.fillStyle="#FF0000";
ctx.strokeStyle="#000000";
ctx.lineWidth=1.0;
ctx.fillRect(x+1,y+1,unitLen/2-2,unitLen/2-2);
//ctx.strokeRect(x,y,unitLen,unitLen);
}


function DrawLine()
{
var i=1;
for(i=1;i<40;i++)
{
	ctx.beginPath();
	ctx.strokeStyle="#000000";
	ctx.lineWidth=1;
	ctx.moveTo(0,i*unitLen);
	ctx.lineTo(canvas.width,i*unitLen);
	ctx.stroke();
	ctx.beginPath();
	ctx.strokeStyle="#000000";
	ctx.lineWidth=1;
	ctx.moveTo(i*unitLen,0);
	ctx.lineTo(i*unitLen,canvas.height);
	ctx.stroke();
}
}
