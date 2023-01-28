outlets = 2;

mgraphics.init();
mgraphics.relative_coords = 1;
mgraphics.autofill = 1;
mgraphics.anti_alias = 1.;

//Load svgs
var gClef = new MGraphicsSVG("jsNote1.svg");
var fClef = new MGraphicsSVG("jsNote2.svg");
var noteHead = new MGraphicsSVG("jsNote3.svg");
var sharpImg = new MGraphicsSVG("jsNote4.svg");
var flatImg = new MGraphicsSVG("jsNote5.svg");
var qSharpImg = new MGraphicsSVG("jsNote6.svg");
var qFlatImg = new MGraphicsSVG("jsNote7.svg");
var tqSharpImg = new MGraphicsSVG("jsNote8.svg");
var tqFlatImg = new MGraphicsSVG("jsNote9.svg");

var notes = new Array();
var noteSpace = 1.5;
declareattribute("noteSpace");
var microTone = 1;
declareattribute("microTone");
var accidents = 0;
var ledger = 0;

var mode = "chord"; //defaults to chords
declareattribute("mode");

var bgcolor = [1.,1.,1., 1.]
declareattribute("bgcolor");
var lineWidth = 1.5;
var width = this.box.rect[2] - this.box.rect[0];
var height = this.box.rect[3] - this.box.rect[1];
var aspect = width / height;

function paint()
{
	with (mgraphics)
	{
		set_source_rgba(bgcolor);
        rectangle_rounded( -aspect, 1, (2 * aspect), 2, 0.1, 0.1);
      	fill();
        select_font_face ("Times", "italic", "bold");
        set_font_size(8);
        set_source_rgba(0, 0, 0, 1);
        move_to(-aspect + 0.04, 0.55)
        text_path("15");
        move_to(-aspect + 0.04, -0.53)
        text_path("15");
        fill();
    }

    //should make all dimensions relative
    if(mode == "chord")
    {
       	drawSlider();
    }

    drawStaff(0.32, gClef, 0.06);
	drawStaff(0.04, gClef, 0.06);
	drawStaff(-0.20, fClef, 0);
	drawStaff(-0.48, fClef, 0);
	drawNotes();	
}

function svg_draw(x, y, s, myFile)
{
	with (mgraphics)
	{
		translate (x, y);
		scale (s, s);
		svg_render(myFile);
		scale (1  / s, 1 / s);
		translate (-1 * x, -1 * y);
	}
}

function list()
{
    notes = new Array();
    notes.length = 0;

    for (var i = 0; i < arguments.length; ++i)
    {
        notes[i] = arguments [i];
    }

    bang();
}

function drawSlider() 
{
    with (mgraphics) 
    {
        set_source_rgba(0.2, 0.2, 0.2, 1.0);
        move_to(-0.9 * aspect, -0.9);
        line_to(0.9 * aspect, -0.9);
        stroke();

        var triX = noteSpace - aspect;
        if (triX > (0.9 * aspect)) triX = 0.9 * aspect;
        if (triX < (-0.9*aspect))  triX = -0.9 * aspect;
        move_to(triX, -0.9);
        line_to(triX - 0.02, -0.85);
        line_to(triX + 0.02, -0.85);
        line_to(triX, -0.9);
        fill();
	}
}


function drawStaff(y, clef, offset)
{
	with (mgraphics) 
	{
		//set_line_width(1.5);
		set_source_rgba(0, 0, 0,1);
		for (var i = 0; i < 5; ++i)
		{
    		move_to(-aspect, y + (i * 0.04));
    		line_to(aspect, y + (i * 0.04));
    	}

    	stroke();
    	y = 1 - y - 0.16;
    	var clefScale = height / 300 * 0.25;
    	svg_draw(0.02, y - offset, clefScale, clef);
    }     
}

function drawNotes()
{
    newX = (0.25 - aspect);
    
	if (notes.length > 0)
    {
     	var i = 0;
     	while (i < notes.length)
        {
            currentDur = notes[i + 1];// not used in chord mode
            currentNote = notes[i];
            currentNote = parseInt(currentNote);
            pitchClass = Math.abs(currentNote % 12);
            fracTone = notes[i] % 1;
            currentOctave = Math.abs(parseInt((currentNote) / 12));
            currentOctave = currentOctave - 5;
            whiteNote = 0; //this is the position on the staff 
            sharp = 0;
            ledger = 0;
            notePosition(pitchClass,fracTone); //figures out what position and accidental to draw

            //Find out what ledger lines are needed
            if (whiteNote == 0 && currentOctave == 0) {ledger = 1}
            else if (whiteNote == 0 && currentOctave == -2) {ledger = -2}
            else if ((whiteNote == 1 || whiteNote == 2) && currentOctave == -2) { ledger = -1 }
            else if ((whiteNote == 5 || whiteNote == 6) && currentOctave ==  1) { ledger = 2 }
            else if (whiteNote == 0 && currentOctave == 2) {ledger = 3}
            else if ((whiteNote == 5 || whiteNote == 6) && currentOctave == 3) {ledger = 4}
            else if ((whiteNote == 0 || whiteNote == 1) && currentOctave == 4) {ledger = 5};
            
           	ypos = 0.98 + ((currentOctave * -0.14) + (whiteNote * -0.02));
            xpos = newX;
            with (mgraphics) 
            {
                //noteheads
                if(mode == "chord")
                {
                	var relXpos = aspect + xpos;
                	var noteScale = height / 300 * 0.25;
                	svg_draw(relXpos, ypos, noteScale, noteHead);
                }
                else
                {
                	if (currentNote == 0)
                	{
                  	 // text(" ");
              		}
               		else
                	{    
                    	var relXpos = (aspect + xpos);
                		var noteScale = height / 300 * 0.25;
                		svg_draw(relXpos, ypos, noteScale, noteHead);
                	}
                	//duration beam
                	durX = xpos + 0.01;
					rectangle(durX, -1  *(ypos - 0.99), currentDur -0.05, 0.02);
                }
                drawAccidentals(relXpos, ypos, sharp);
                drawLedgerLines(ledger,xpos);
            }
      
            
            if(mode=="chord")
            {
            	newX = newX  +(noteSpace * 0.1);
            	++i;
            }
            else
            {
            	newX = newX + currentDur;
            	i += 2;
            }          
        }
    }
}

function drawAccidentals(xpos,ypos,sharp)
{
    with (mgraphics) 
    {
    	var offX = 0.05;
    	var sharpY = 0.04;
    	var flatY = 0.06;
    	var acciScale = height / 300 * 0.25;
        
		switch(sharp)
        {    
            case 0.25:
                text("V");
                break;    
            case 0.5:
                svg_draw(xpos - offX, ypos - sharpY, acciScale, qSharpImg);
                break;    
            case .75:
                text("`");
                break;    
            case 1:
            	svg_draw(xpos - offX, ypos - sharpY, acciScale, sharpImg);
                break;    
            case 1.25:
                text("h");
                break;    
            case 1.5:
                svg_draw(xpos - offX, ypos - sharpY, acciScale, tqSharpImg);
                break;    
            case 1.75:
                text("s");
                break;    
            case -0.25:
                text("2");
                break;
            case -0.5:
                svg_draw(xpos - offX, ypos - flatY, acciScale, qFlatImg);
                break;    
            case -0.75:
                text("<");
                break;    
            case -1:
                svg_draw(xpos - offX, ypos - flatY, acciScale, flatImg);
                break;
            case -1.25:
                text("F");
                break;
            case -1.5:
                svg_draw(xpos - offX - 0.02, ypos - flatY, acciScale, tqFlatImg);
                break;
            case -1.75:
                text("P");
                break;
    	}
    }
}

function drawLedgerLines(ledger, x)
{    
    var ledgerLeft = 0.02;
    var ledgerRight = 0.07;

    with (mgraphics) 
    {
        switch(ledger)
        {
       		case 1:
        		move_to(x - 0.02, 0);
    			line_to(x + 0.07, 0);
            	break;    
        	case -1:
        		move_to(x - ledgerLeft, -0.24);
    			line_to(x + ledgerRight, -0.24);
            	break;    
        	case -2:
            	move_to(x - ledgerLeft, -0.24);
    			line_to(x + ledgerRight, -0.24);
            	move_to(x - ledgerLeft, -.28);
    			line_to(x + ledgerRight, -.28);
            	break;    
        	case 2:
        		move_to(x - ledgerLeft, 0.24);
    			line_to(x + ledgerRight, 0.24);
            	break;    
        	case 3:
            	move_to(x - ledgerLeft, 0.24);
    			line_to(x + ledgerRight, 0.24);
            	move_to(x - ledgerLeft, 0.28);
    			line_to(x + ledgerRight, 0.28);
            	break;
        	case 4:
            	move_to(x - ledgerLeft, 0.52);
    			line_to(x + ledgerRight, 0.52);
            	break;
        	case 5:
            	move_to(x - ledgerLeft, 0.52);
    			line_to(x + ledgerRight, 0.52);
    			move_to(x - ledgerLeft, 0.56);
    			line_to(x + ledgerRight, 0.56);
            	break;       
        }
        stroke();
    }
}

function notePosition(pitchClass, fracTone) //This takes the pitchClass and figures out what position and accidental it needs
{
    switch(pitchClass) //setting correctly for sharp case
    {
		case 0:
        	whiteNote = 0;
           	break;
        case 1:
          	whiteNote = 0;
            sharp = 1;
            break;
        case 2:
        	whiteNote = 1;
            break;
        case 3:
            whiteNote = 1;
            sharp = 1;
            break;
        case 4:
            whiteNote = 2;
            break;
        case 5:
            whiteNote = 3;
            break;
        case 6:
            whiteNote = 3;
         	sharp = 1;
           	break;
      	case 7:
         	whiteNote = 4;
           	break;
       	case 8:
          	whiteNote = 4;
            sharp = 1;
            break;
      	case 9:
          	whiteNote = 5;
           	break;
        case 10:
        	whiteNote = 5;
    		sharp = 1;
      		break;
       	case 11:
          	whiteNote = 6;
            break;
 	}
   	
	if (currentNote < 0 && sharp == 1) //fix if should be flat
   	{
   		whiteNote = whiteNote + 1;
   		sharp = -1;
   	}

    //for microtones
    switch (microTone)
    {
        case 1: //quarter tones
            if (fracTone < .75 && fracTone > .25)
            {
                sharp = sharp + 0.5;
            }
            else if (fracTone >= 0.75)
            {
                //currentNote = currentNote+2.;
                sharp = sharp + 1.;
            }
            else if (fracTone < -0.25 && fracTone > -0.75  && sharp == -1)
            {
               	sharp = sharp + 0.5;
            }
            else if (-0.75 < fracTone && fracTone < -0.25)
            {
              	sharp = -1.5;
            }
            else if (fracTone < -0.75)
            {        	
            }
            break;
        case 3: //eighth tones
            if (fracTone > 0.125 && fracTone < 0.375)
            {
                sharp = sharp + 0.25;
            }
            else if (fracTone > 0.375 && fracTone < 0.625)
            {
                sharp = sharp + 0.5;
            }
            else if (fracTone > 0.625 && fracTone < 0.875)
            {
                sharp = sharp+0.75
            }    
            else if (fracTone >.875)
            {
                sharp = sharp + 1;
            }
            break;
        }

        if (sharp == 2)
        {
            currentNote = currentNote + 2;
            sharp = 0;
        }
}

function space(s)
{
    noteSpace = s;
    bang();
}

function accidentals(a)
{
    accidents = a;
    bang();
}

function micromode(m)
{
    microTone = m;
    bang();
}

function displaymode(d)
{
	notes.length = 0;
	mode = d;
	bang();
}

function backgroundcolor(r, g, b, a)
{
	bgcolor = [r, g, b, a];
	bang();
}

function bang()
{
    mgraphics.redraw();
    outlet(1, notes[0]);
    tsk = new Task(rhythmicOutput); 
    outRate = 100 * noteSpace;
    tsk.interval = outRate;
    tsk.repeat(notes.length, outRate); 
    outlet(0,notes);
}

function rhythmicOutput() 
{ 
	outlet(1, notes[arguments.callee.task.iterations]); 
} 

function onresize(w, h)
{
    width = this.box.rect[2] - this.box.rect[0];
	height = this.box.rect[3] - this.box.rect[1];
	aspect = width / height;
	bang();
}
onresize.local = 1; //private

function onclick(x, y, but, cmd, shift, capslock, option, ctrl)
{
    ondrag(x, y, but, cmd, shift, capslock, option, ctrl);
    if (drag[1]>-0.8) bang();
}
onclick.local = 1; //private

function ondrag(x, y, but, cmd, shift, capslock, option, ctrl)
{
    if (mode == "chord")
    {
    	drag = sketch.screentoworld(x, y);
    	if (drag[1] <  -0.8)
    	{
    		noteSpace = (drag[0]  + aspect);
       	}
    	if (noteSpace < 0.)
    	{
        	noteSpace = 0.;
        }
   		if (noteSpace > (2  * aspect))
   		{
        	noteSpace = 2  * aspect;
        }
    	mgraphics.redraw();
    }
}
ondrag.local = 1; //private
