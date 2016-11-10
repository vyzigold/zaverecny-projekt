//var barvy = Object(); 

var proudari = {
	muzi:{
		pProud:[],
		lProud:[],
		day:{
			datum:[],
			pocet:[]
		}
	},
	zeny:{
		pProud:[],
		lProud:[],
		day:{
			datum:[],
			pocet:[]
		}
	}
}
var data;
function getMousePos(canvas, evt) 
{
	var rect = canvas.getBoundingClientRect();
    return {
        x: evt.clientX - rect.left, y: evt.clientY - rect.top
    };
}

function pixelOnMouseOver(canvas,callback)
{
	ctx = canvas.getContext("2d");
	canvas.addEventListener('mousemove',function(e){
		var w = canvas.width, h=canvas.height;
		data= ctx.getImageData(0,0,w,h).data;
		if (e.offsetX === void 0) {
			Object.defineProperties(MouseEvent.prototype, {
				'offsetX': {
					get: function() {
						return this.layerX - this.target.offsetLeft;
					}
				}, 'offsetY': {
					get: function() {
						return this.layerY - this.target.offsetTop;
					}
				}
			});
		}
		var idx = (e.offsetY*w + e.offsetX)*4;
		var parts = Array.prototype.slice.call(data,idx,idx+4);
		mousePos = getMousePos(canvas, e);
		//document.getElementById("bbb").innerHTML=Math.floor(mousePos.x/100)+1;
		callback.apply(ctx,parts);
	},false);
}


function mouse()
{
	return Math.floor(mousePos.x/100);
}


function addProudari(pohlavi, pProud, lProud, datum)
{
	var found = false;
	proudari[pohlavi].pProud[proudari[pohlavi].pProud.length]=pProud;
	proudari[pohlavi].lProud[proudari[pohlavi].lProud.length]=lProud;
	for(var i = 0; i < proudari[pohlavi].day.datum.length && !found; i++)
	{
		if(proudari[pohlavi].day.datum[i] == datum)
		{
			found = true;
			proudari[pohlavi].day.pocet[i]++;
		}
	}
	if(!found)
	{
		proudari[pohlavi].day.datum[proudari[pohlavi].day.datum.length] = datum;
		proudari[pohlavi].day.pocet[proudari[pohlavi].day.pocet.length] = 1;
	}
}

/*function addDay(pohlavi,datum,pocet)
{
	proudari[pohlavi].day.datum[proudari[pohlavi].day.datum.length] = datum;
	proudari[pohlavi].day.pocet[proudari[pohlavi].day.pocet.length] = pocet;
}*/

function printStupnice(canvas,pohlavi)
{
	var c = canvas.getContext("2d");
	var vyska = canvas.height;
	var sirka = canvas.width;
	var max=0;
	var min=999999;
	var last=0;
	for(var i = 0; i <proudari[pohlavi].pProud.length;i++)
	{
		if(proudari[pohlavi].pProud[i] > max)
			max = proudari[pohlavi].pProud[i];
		if(proudari[pohlavi].lProud[i] > max)
			max = proudari[pohlavi].lProud[i];
		if(proudari[pohlavi].pProud[i] < min)
			min = proudari[pohlavi].pProud[i];
		if(proudari[pohlavi].lProud[i] < min)
			min = proudari[pohlavi].lProud[i];
	}
	var pocetUtoku = 0;
	var pozice = 0;
	var velikostPisma = 0;
	if(vyska > sirka)
		velikostPisma = sirka/32;
	else
		velikostPisma = vyska/32;
	for(var j=0;j<proudari[pohlavi].day.datum.length;j++)
	{
		pozice = pocetUtoku*(sirka/proudari[pohlavi].pProud.length)
		poziceTextu = Math.floor((pozice+(pocetUtoku+proudari[pohlavi].day.pocet[j])*(sirka/proudari[pohlavi].pProud.length))/2-2.2*velikostPisma);
		c.beginPath();
		c.moveTo(pozice,vyska);
		c.strokeStyle="rgb(0,0,0)";
		c.lineTo(pozice,vyska-velikostPisma-5);
		c.lineWidth = 2;
		c.lineCap   = 'round';	
		c.stroke();
		c.font = velikostPisma+"px Arial";
		c.fillText(proudari[pohlavi].day.datum[j],poziceTextu,vyska);
		pocetUtoku+=proudari[pohlavi].day.pocet[j];
	}
	var i = min;
	var cislo;
	do
	{
		if(i > max)
			i = max;
		pozice = vyska-(vyska/(max-min)*(i-min))
		c.beginPath();
		c.moveTo(0,pozice)
		c.strokeStyle="rgb(0,0,0)";
		c.lineTo(velikostPisma+5,pozice);
		c.lineWidth = 2;
		c.lineCap   = 'round';	
		c.stroke();
		c.font = velikostPisma+"px Arial";
		cislo = (i/100).toString();
		if(cislo.length == 4)
			cislo += "0";
		if(cislo.length == 2)
			cislo += ".00"
		if(i == min)
			c.fillText(cislo,0,pozice-2);
		else if(i == max)
			c.fillText(cislo,velikostPisma*2.3,pozice+0.7*velikostPisma);
		else
			c.fillText(cislo,0,pozice-2);
		/*if(i%100 != 0 && i < max)
		{
			i = Math.round(i/100);
			if(i%2 == 1)
				i-=1;
			i*=100;
		}*/
		console.log(i);
		if(i<max)
			i+=Math.round(((max-min)/15)/10)*10;
		if(i==max)
			i = 9999;
		if((i > max && i != 9999)|| (max - i < velikostPisma && i != max && max - i > 0))
			i = max;
	}while(i!=9999)
}

function printProudari(canvas,zvirazni,pohlavi)
{
	var c = canvas.getContext("2d");
	//c.fillStyle="rgba(0,255,0,1)";
	//c.fillRect(0,0,canvas.width,canvas.height);
	var vyska = canvas.height;
	var sirka = canvas.width;
	var max=0;
	var min=999999;
	var last=0;
	for(var i = 0; i <proudari[pohlavi].pProud.length;i++)
	{
		if(proudari[pohlavi].pProud[i] > max)
			max = proudari[pohlavi].pProud[i];
		if(proudari[pohlavi].lProud[i] > max)
			max = proudari[pohlavi].lProud[i];
		if(proudari[pohlavi].pProud[i] < min)
			min = proudari[pohlavi].pProud[i];
		if(proudari[pohlavi].lProud[i] < min)
			min = proudari[pohlavi].lProud[i];
	}
	c.clearRect(0,0,sirka,vyska);
	for(var j=0;j<proudari[pohlavi].pProud.length;j++)
	{
		c.beginPath();
		if(j==0)
			c.moveTo(0,vyska);
		else
			c.moveTo(sirka/proudari[pohlavi].pProud.length*j,vyska-(vyska/(max-min)*(proudari[pohlavi].pProud[j-1]-min)));
		c.strokeStyle="rgba(255,0,0,0.005)";
		c.lineTo((sirka/proudari[pohlavi].pProud.length)*(j+1),vyska-(vyska/(max-min)*(proudari[pohlavi].pProud[j]-min)));
			c.lineWidth = 20;
		c.lineCap   = 'round';	
		c.stroke();
	}
	for(var j=0;j<proudari[pohlavi].lProud.length;j++)
	{
		c.beginPath();
		if(j==0)
			c.moveTo(0,vyska);
		else
			c.moveTo(sirka/proudari[pohlavi].pProud.length*j,vyska-(vyska/(max-min)*(proudari[pohlavi].lProud[j-1]-min)));
		c.strokeStyle="rgba(0,0,255,0.005)";
		c.lineTo(sirka/proudari[pohlavi].pProud.length*(j+1),vyska-(vyska/(max-min)*(proudari[pohlavi].lProud[j]-min)));
			c.lineWidth = 20;
		c.lineCap   = 'round';	
		c.stroke();
	}
	for(var j=0;j<proudari[pohlavi].pProud.length;j++)
	{
		c.beginPath();
		if(j==0)
			c.moveTo(0,vyska);
		else
			c.moveTo(sirka/proudari[pohlavi].pProud.length*j,vyska-(vyska/(max-min)*(proudari[pohlavi].pProud[j-1]-min)));
		c.strokeStyle="rgb(255,0,0)";
		c.lineTo((sirka/proudari[pohlavi].pProud.length)*(j+1),vyska-(vyska/(max-min)*(proudari[pohlavi].pProud[j]-min)));
		if("pravy"==zvirazni)
			c.lineWidth = 10;
		else
			c.lineWidth = 2;
		c.lineCap   = 'round';	
		c.stroke();
	}
	for(var j=0;j<proudari[pohlavi].lProud.length;j++)
	{
		c.beginPath();
		if(j==0)
			c.moveTo(0,vyska);
		else
			c.moveTo(sirka/proudari[pohlavi].pProud.length*j,vyska-(vyska/(max-min)*(proudari[pohlavi].lProud[j-1]-min)));
		c.strokeStyle="rgb(0,0,255)";
		c.lineTo(sirka/proudari[pohlavi].pProud.length*(j+1),vyska-(vyska/(max-min)*(proudari[pohlavi].lProud[j]-min)));
		if("levy"==zvirazni)
			c.lineWidth = 10;
		else
			c.lineWidth = 2;
		c.lineCap   = 'round';	
		c.stroke();
	}
	printStupnice(canvas,pohlavi);
}