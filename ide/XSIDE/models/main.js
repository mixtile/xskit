function AppMain()
{
    canvas = new Canvas();
    ctx=canvas.getContext('2d');

    ctx.font = "20px sans-serif";
    ctx.fillStyle="#FF0000";
    ctx.fillText("Hello World!", canvas.width/2, canvas.height/2, 20);
}

AppMain();
