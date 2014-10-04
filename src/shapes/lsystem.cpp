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

string toupper(string s){
	int c = 0;
	std::vector<char> ruleChars;
	string ret ("");
	for(size_t i=0;i<s.length();i++){
		ret += toupper(s[i]);
	}
	return ret;
}
string tolower(string s){
	int c = 0;
	std::vector<char> ruleChars;
	string ret ("");
	for(size_t i=0;i<s.length();i++){
		ret += tolower(s[i]);
	}
	return ret;
}

//ABAAB
//abBababB
//abaababa
/**
* Rules are a (key,val) pair: A->A+A
* Applying a rule means taking the string and making the appropriate substitutions.
*/
void LSystem::apply_rule(string &tmp, char key, string value){
	size_t len = value.length();

	for(size_t i=0; i<tmp.length(); i++){
		if(tmp[i]==key){
			tmp = tmp.substr(0,i) + tolower(value) + tmp.substr(i+1,tmp.length());
		}
	}
}

void LSystem::generate(){
	map<char,string>::iterator it;
	cout << steps << " " << delta << "\n";
	string str;
	str = axiom;

	cout << "axiom: " << axiom << "\n";	
	str = toupper(str);
	for(int i=0; i<steps;i++){
		for(it = rules.begin(); it != rules.end(); ++it){
			apply_rule(str, it->first, it->second);
		}
		str = toupper(str);
		cout << str << endl;
	}
	cout << "output: " << str << "\n";
}

BBox LSystem::ObjectBound() const {
    BBox objectBounds;
    float radius,zmin,zmax;
    radius = 5.0;
    zmin = 0.0;
    zmax = 90.0;
    Point p1 = Point(-radius, -radius, zmin);
    Point p2 = Point( radius,  radius, zmax);
    return BBox(p1, p2);
    //return objectBounds;
}

#include "transform.h"
void LSystem::Refine(vector<Reference<Shape> > &refined) const {
   /* for (int i = 0; i < ntris; ++i)
        refined.push_back(new Triangle(ObjectToWorld,
                          WorldToObject, ReverseOrientation,
                          (TriangleMesh *)this, i));*/
    float radius = 15.0;
    float zmin = 0.0;
    float zmax = 90.0;
    float phimax = 360.0;
    refined.push_back(new Cylinder(ObjectToWorld, WorldToObject, ReverseOrientation, 
    	radius, zmin, zmax, phimax));
    radius = 5.0;
    zmax = 130.0;

    const Transform &o2w = *ObjectToWorld;
    Transform *identity;
    refined.push_back(new Cylinder(ObjectToWorld,WorldToObject, ReverseOrientation, 
    	radius, zmin, zmax, phimax));
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
	return new LSystem(o2w,w2o,reverseOrientation,axiom,rule_map,steps,delta);
}