// replacing in a string

#include "stdafx.h"
#include "shapes/lsystem.h"
#include "shapes/cylinder.h"
#include "paramset.h"
#include <iostream>
/*
#include <iostream>
#include <string>
#include <vector>
#include <map>*/

using namespace std;

LSystem::LSystem(const Transform *o2w, const Transform *w2o, bool ro,
	string a, map<char, string> r, int s, float d) : Shape(o2w, w2o, ro) {
	axiom = a;
	rules = r;
	steps = s;
	delta = d;
	generate();
}

void LSystem::generate(){
	map<char,string>::iterator it;
	bool found;

	cout << "# axiom: " << axiom << endl;
	string current = axiom;
	string next = "";
	for(int i=0;i<steps;i++){
		for(size_t j=0; j<current.length();j++){
			found = false;
			for(it = rules.begin(); it != rules.end(); ++it){
				if(current[j] == it->first){
					next = next + it->second;
					found = true;
					break;
				}
			}
			if(!found){
				next = next + current[j];
			}
		}
		current = next;
		next = "";
		cout << "# " << current << endl;
	}
	generated_system = current;
}

BBox LSystem::ObjectBound() const {
    BBox objectBounds;
    float radius,zmin,zmax;
    radius = 5.0;
    zmin = 0.0;
    zmax = 190.0;
    Point p1 = Point(-radius, -radius, zmin);
    Point p2 = Point( radius,  radius, zmax);
    return BBox(p1, p2);
    //return objectBounds;
}


void LSystem::Refine(vector<Reference<Shape> > &refined) const {

}

#include "api.h"
#include "paramset.h"
void makePBRTSceneFile(string lsystem, float delta){
	ParamSet params;
	float cylRadius = 1.0;
	float drawSize = 1.0;
	float shrinkFactor = 1.4;
	float zero = 0.0;

	for(size_t i=0; i<lsystem.length(); i++){
		switch(lsystem[i]){
			case 'F':
			case 'f':
				params.AddFloat("radius", &cylRadius, 1);
				params.AddFloat("zmin", &zero,1);
				params.AddFloat("zmax", &drawSize,1);
				pbrtShape("cylinder", params);
				pbrtTranslate(0.0, 0.0, drawSize);
				break;
			case '+':
				pbrtRotate(delta, 0.0, 1.0, 0.0);
				break;
			case '-':
				pbrtRotate(-delta, 0.0, 1.0, 0.0);
				break;
			case '&':
				pbrtRotate(delta, 1.0, 0.0, 0.0);
				break;
			case '^':
				pbrtRotate(-delta, 1.0, 0.0, 0.0);
				break;
			case '\\':
				pbrtRotate(delta, 0.0, 0.0, 1.0);
				break;
			case '/':
				pbrtRotate(-delta, 0.0, 0.0, 1.0);
				break;
			case '|':
				pbrtRotate(180.0, 0.0, 1.0, 0.0);
				break;
			case '[':
				cylRadius = cylRadius / shrinkFactor;
				pbrtAttributeBegin();
				pbrtTranslate(0.0, 0.0, cylRadius);
				break;
			case ']':
				cylRadius = cylRadius * shrinkFactor;
				pbrtAttributeEnd();
				break;
			default:
			// Ignoring symbol

			break;
		}
	}
}

LSystem *CreateLSystemShape(const Transform *o2w, const Transform *w2o,
        bool reverseOrientation, const ParamSet &params){
	int nr;
	map<char, string> rule_map;

	string axiom = params.FindOneString("axiom", "B");
	const string *rules = params.FindString("rules",&nr);
	int steps = params.FindOneInt("steps", 1);
	float delta = params.FindOneFloat("delta", 60);
	
	char key;
	string val;
	/* Rule is <char>-><string> which is key->val*/
	for(int i=0; i<nr;i++){
		key = rules[i][0];
		val = rules[i].substr(3,rules[i].length());
		rule_map[key] = val;
		cout << rule_map[key] << endl;
	}
	LSystem *s = new LSystem(o2w,w2o,reverseOrientation,axiom,rule_map,steps,delta);
	makePBRTSceneFile(s->generated_system, s->delta);
	return s;
}