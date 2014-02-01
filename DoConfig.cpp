/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The F*** You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
#include "FL/Fl.H"
#include "FL/Fl_Window.H"
#include "FL/Fl_Radio_Round_Button.H"
#include "FL/Fl_Choice.H"
#include "FL/Fl_Check_Button.H"
#include "SDL/SDL.h"

#define HEADER "DOUKUTSU20041206"
#define TEXT "Courier New"

struct data{
	char header[32];
	char text[64];
	int move;
	int attack;
	int okay;
	int display;
	int useJoy;
	int buttons[8];
};

class RadioRow{
	public:
		RadioRow(char offset);
		int value();
		void value(int input);
	private:
		Fl_Group *group;
		Fl_Radio_Round_Button *buttons[6];
		Fl_Group *label;
};

RadioRow::RadioRow(char offset){
	char *temp = new char[2];
	*(temp) = (char)(49+offset); //Muhahahahahahah!
	*(temp+1) = '\0';
	this->group = new Fl_Group(110+offset*30, 150, 30, 180);
	this->group->label(temp);
	this->group->align(FL_ALIGN_TOP_LEFT);
	for(char i=0;i<6;i++){
		this->buttons[i] = new Fl_Radio_Round_Button(110+offset*30, 150+30*i, 30, 30);
	}
	this->group->end();
}

int RadioRow::value(){
	char i;
	for(i=0;i<6;i++){
		if(this->buttons[i]->value()){
			return (int)i+1;
		}
	}
	return 0;
}

void RadioRow::value(int input){
	this->buttons[input-1]->setonly();
}

const char* ActionName(int actionNumber){
	switch (actionNumber) {
		case 1: return "Jump";
		case 2: return "Attack";
		case 3: return "Weapon+";
		case 4: return "Weapon-";
		case 5: return "Item";
		case 6: return "Map";
	}
	return "<unknown>";
}

#ifdef HAVE_SDL
class JoystickReader{
public:
	friend class JoystickSingleton;
	~JoystickReader();

	typedef bool (*ButtonSelectedCb) (int,void*);
	void callback(ButtonSelectedCb callbackFunc, void* callbackUserdata);
private:
	JoystickReader(SDL_Joystick* js_);
	static void OnTimer (void* userdata);

	SDL_Joystick* js;
	ButtonSelectedCb callbackFunc;
	void* callbackUserdata;
};

class JoystickSingleton {
public:
	static JoystickSingleton& Instance();
	JoystickReader* CreateReader();
	int NumJoysticks() const;
	const char* Name() const;
private:
	JoystickSingleton();
	SDL_Joystick* js;
};


JoystickReader::JoystickReader(SDL_Joystick* js_)
: js(js_), callbackFunc(NULL), callbackUserdata(NULL)
{
	Fl::add_timeout(0.1, OnTimer, this);
}

JoystickReader::~JoystickReader(){
	Fl::remove_timeout(OnTimer, this);
}

void JoystickReader::callback(ButtonSelectedCb callbackFunc_, void* callbackUserdata_){
	callbackFunc = callbackFunc_;
	callbackUserdata = callbackUserdata_;
}

void JoystickReader::OnTimer(void* userdata){
	JoystickReader* self = (JoystickReader*)userdata;
	SDL_JoystickUpdate();
	const int numButtons = SDL_JoystickNumButtons(self->js);
	for (int i = 0; i < numButtons; i++) {
		if (SDL_JoystickGetButton(self->js, i)){
			if (self->callbackFunc)
				if (self->callbackFunc(i, self->callbackUserdata))
					return;
		}
	}
	Fl::repeat_timeout(0.1, OnTimer, self);
}


JoystickSingleton& JoystickSingleton::Instance(){
	static JoystickSingleton staticInstance;
	return staticInstance;
}

JoystickSingleton::JoystickSingleton(){
	SDL_Init(SDL_INIT_JOYSTICK);
	if (SDL_NumJoysticks() == 0)
		return;

	js = SDL_JoystickOpen(0); // always use the first joystick
	SDL_JoystickEventState(SDL_QUERY);
}

JoystickReader* JoystickSingleton::CreateReader(){
	return new JoystickReader(js);
}

int JoystickSingleton::NumJoysticks() const{
	return SDL_NumJoysticks();
}

const char* JoystickSingleton::Name() const{
	return SDL_JoystickName(0);
}
#endif


Fl_Round_Button *movear;
Fl_Round_Button *movegt;

Fl_Round_Button *buttonxz;
Fl_Round_Button *buttonzx;

Fl_Round_Button *okayjump;
Fl_Round_Button *okayattack;

Fl_Choice *displaychoice;
Fl_Check_Button *joychoice;

Fl_Group *joystuffcontainer;
RadioRow *joyRows[8];


#ifdef HAVE_SDL
class ButtonSelectionDialog {
public:
	ButtonSelectionDialog(int actionNumber);
	~ButtonSelectionDialog();

private:
	static bool OnButtonSelected(int button, void* userdata);
	static void OnCancelButton(Fl_Widget*, void* userdata);
	static void OnClose (Fl_Widget*, void* userdata);

	int actionNumber;
	Fl_Window* win;
	char labelText[100];
	Fl_Group* errorLabel;
	char errorLabelText[100];
	JoystickReader* buttonReader;
};

ButtonSelectionDialog::ButtonSelectionDialog(int actionNumber_){
	actionNumber = actionNumber_;
	buttonReader = JoystickSingleton::Instance().CreateReader();
	buttonReader->callback(OnButtonSelected, this);

	win = new Fl_Window(550, 130, "Press Joystick Button");
	sprintf(labelText, "Press button for \"%s\" on joystick \"%s\"", ActionName(actionNumber), JoystickSingleton::Instance().Name());
	Fl_Group* label = new Fl_Group(10, 35, 530, 5, labelText);
	label->end();
	errorLabel = new Fl_Group(15, 70, 530, 5);
	errorLabel->labelcolor((Fl_Color)1);
	errorLabel->hide();
	errorLabel->end();
	Fl_Button* btn = new Fl_Button(435, 90, 100, 25, "Cancel");
	btn->callback(OnClose, this);
	win->callback(OnClose, this);
	win->end();
	win->set_modal();
	win->show();
}

ButtonSelectionDialog::~ButtonSelectionDialog(){
	delete buttonReader;
	delete win;
}

bool ButtonSelectionDialog::OnButtonSelected(int button, void* userdata){
	ButtonSelectionDialog* self = (ButtonSelectionDialog*)userdata;
	if (button >= 8){
		sprintf(self->errorLabelText, "Button %d not supported!", button);
		self->errorLabel->label(self->errorLabelText);
		self->errorLabel->show();
		return false;
	} else {
		joyRows[button]->value(self->actionNumber);
		delete self;
		return true;
	}
}

void ButtonSelectionDialog::OnClose (Fl_Widget*, void* userdata){
	ButtonSelectionDialog* self = (ButtonSelectionDialog*)userdata;
	delete self;
}
#endif

void quit(Fl_Widget*, void*){
	std::exit(0);
}

void activatejoy(Fl_Widget*, void*){
	if(joystuffcontainer->active()){
		joystuffcontainer->deactivate();
	} else {
		joystuffcontainer->activate();
	}
}

#ifdef HAVE_SDL
ButtonSelectionDialog* buttonSelector = NULL;
void pickJsButton(Fl_Widget*, void* userdata){
	int actionNumber = (int)userdata;
	buttonSelector = new ButtonSelectionDialog(actionNumber);
}
#endif

void read_Config(){
	std::fstream fd;
	data config;
	fd.open("Config.dat", std::ios::in | std::ios::binary);
	fd.read((char*)&config, 148);
	if (config.move == 0){
		movear->setonly();
	} else {
		movegt->setonly();
	}
	if (config.attack == 0){
		buttonxz->setonly();
	} else { 
		buttonzx->setonly();
	}
	if (config.okay == 0){
		okayjump->setonly();
	}else{
		okayattack->setonly();
	}
	displaychoice->value(config.display);
	joychoice->value(config.useJoy);
	if( !config.useJoy ){
		joystuffcontainer->deactivate();
	}
	for(char i=0;i<8;i++){
		if(config.buttons[i]<9 && config.buttons[i]>0){
			joyRows[i]->value(config.buttons[i]);
		}
	}
	fd.close();
}


void write_Config(Fl_Widget*, void*){
	std::fstream fd;
	data config;
	std::memset(config.header, '\0', 32);
	std::memset(config.text, '\0', 64);
	std::strcpy(config.header, HEADER);
	std::strcpy(config.text, TEXT);
	fd.open("Config.dat", std::ios::out | std::ios::binary);
	
	config.move = movegt->value();
	config.attack = buttonzx->value();
	config.okay = okayattack->value();
	
	config.display = displaychoice->value();
	config.useJoy = joychoice->value();
	for(char i =0;i<8;i++){
		config.buttons[i] = joyRows[i]->value();
	}
	fd.write((char*)&config, 148);
	fd.close();
	exit(0);
}
int main(int argc, char* argv[]){
	Fl_Window *mainw = new Fl_Window(400, 380, "DoConfigure - Doukutsu Monotagari Settings");
	
	Fl_Group *movegroup = new Fl_Group(10, 10, 185, 50);
		movegroup->box(FL_THIN_DOWN_BOX);
		movear = new Fl_Radio_Round_Button(10, 10, 185, 20, "Arrows for Movement");
		movear->setonly();
		movegt = new Fl_Radio_Round_Button(10, 40, 185, 20, "<>? for Movement");
	movegroup->end();
	
	Fl_Group *buttongroup = new Fl_Group(10, 70, 185, 50);
		buttongroup->box(FL_THIN_DOWN_BOX);
		buttonxz = new Fl_Radio_Round_Button(10, 70, 185, 20, "Z=Jump; X=Attack");
		buttonxz->setonly();
		buttonzx = new Fl_Radio_Round_Button(10, 100, 185, 20, "X=Jump; Z=Attack");
	buttongroup->end();
	
	Fl_Group *okaygroup = new Fl_Group(205, 10, 185, 50);
		okaygroup->box(FL_THIN_DOWN_BOX);
		okayjump = new Fl_Radio_Round_Button(205, 10, 185, 20, "Jump=Okay");
		okayjump->setonly();
		okayattack = new Fl_Radio_Round_Button(205, 40, 185, 20, "Attack=Okay");
	okaygroup->end();
	
	displaychoice = new Fl_Choice(205, 70, 185, 20);
	Fl_Menu_Item screens[] = {
		{"Fullscreen 16-bit"},
		{"Windowed 320x240"},
 		{"Windowed 640x480"},
		{"Fullscreen 24-bit"},
		{"Fullscreen 32-bit"},
		{0}};
	displaychoice->menu(screens);
	joychoice = new Fl_Check_Button(205, 100, 185, 20, "Use Joypad");
	joychoice->callback(&activatejoy);
		
	joystuffcontainer = new Fl_Group(10, 130, 380, 200);
		joystuffcontainer->box(FL_THIN_DOWN_BOX);
		for(char i=0;i<8;i++){
			joyRows[i] = new RadioRow(i);
		}
		//There's no Label class alright? I'll switch it as soon as one is introduced.
		for (char i=0;i<6;i++) {
			Fl_Group *labeljump = new Fl_Group(10, 150+i*30, 10, 20);
			char* label=new char[strlen(ActionName(i+1))+2];
			sprintf(label, "%s:", ActionName(i+1));
			labeljump->label(label);
			labeljump->align(FL_ALIGN_RIGHT);
			labeljump->end();
		}
		
#ifdef HAVE_SDL
		if (JoystickSingleton::Instance().NumJoysticks() > 0){
			for (char i=0;i<6;i++){
				Fl_Button* btn = new Fl_Button(345, 150+30*i, 35, 20, "pick");
				btn->callback(&pickJsButton, (void*)(long)(i+1));
			}
		}
#endif

	joystuffcontainer->end();
	
	Fl_Button *okaybutton = new Fl_Button(10, 340, 185, 30, "Okay");
	okaybutton->callback(&write_Config);
	Fl_Button *cancelbutton = new Fl_Button(205, 340, 185, 30, "Cancel");
	cancelbutton->callback(&quit);
	
	mainw->end();
	mainw->show(argc, argv);
	
#ifdef HAVE_SDL
	char joystickLabel[100];
	sprintf(joystickLabel, "%s (%d found)", joychoice->label(), JoystickSingleton::Instance().NumJoysticks());
	joychoice->label(joystickLabel);
#endif

	read_Config();
	Fl::option(Fl::OPTION_VISIBLE_FOCUS, false);
	return Fl::run();
}
