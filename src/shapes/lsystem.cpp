#include "stdafx.h"
#include "shapes/lsystem.h"
#include "shapes/cylinder.h"
#include "paramset.h"
#include <iostream>
/*#include <random>*/
#include <cstdlib>
#include <ctime>

using namespace std;

LSystem::LSystem(const Transform *o2w, const Transform *w2o, bool ro,
	string a, RuleMap r, int s, float d) : Shape(o2w, w2o, ro) {
	axiom = a;
	rules = r;
	steps = s;
	delta = d;
	generate();
}

/*double rand(){
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(0.0,1.0);
	return distribution(generator);
}*/


static int initTime = 1;
void LSystem::generate(){
	RuleMap::iterator it;
	bool found;
	float randomNumber;
	float begin,end;

	if(initTime){
		srand(time(NULL));
		initTime = 0;
	}

	/*cout << "# axiom: " << axiom << endl;*/
	string current = axiom;
	string next = "";
	string replacement;
	for(int i=0;i<steps;i++){
		for(size_t j=0; j<current.length();j++){
			found = false;
			for(it = rules.begin(); it != rules.end(); ++it){
				if(current[j] == it->first){
					if(rules[it->first].size() > 1){
						/* Choose among possibilities */
						randomNumber = ((double) rand() / (RAND_MAX));
						/*cout << randomNumber << endl;*/
						begin = end = 0.0;
						for(size_t j=0; j<rules[it->first].size();j++){
							end += it->second[j].second;
							/*cout << begin << " " << end << endl;*/
							if(randomNumber > begin && randomNumber < end){
								replacement =  it->second[j].first;
								/*cout << "got" << replacement << endl;*/
								break;
							}else{
								begin = end;
							}
						}
					}else{
						replacement = it->second[0].first;
					}
					next = next + replacement;
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
		/*cout << "# " << current << endl;*/
	}
	generated_system = current;
	/*cout << "# " << generated_system << endl;*/
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
void makePBRTSceneFile(string lsystem, float delta, float cylinderRadius){
	ParamSet params;
	float drawSize = 1.0;
	float shrinkFactor = 1.4;
	float zero = 0.0;

	for(size_t i=0; i<lsystem.length(); i++){
		switch(lsystem[i]){
			case 'F':
			case 'f':
				params.AddFloat("radius", &cylinderRadius, 1);
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
				cylinderRadius = cylinderRadius / shrinkFactor;
				pbrtAttributeBegin();
				pbrtTranslate(0.0, 0.0, cylinderRadius);
				break;
			case ']':
				cylinderRadius = cylinderRadius * shrinkFactor;
				pbrtAttributeEnd();
				break;
			default:
				break;
		}
	}
}

LSystem *CreateLSystemShape(const Transform *o2w, const Transform *w2o,
        bool reverseOrientation, const ParamSet &params){
	int nr;
	RuleMap rule_map;

	string axiom = params.FindOneString("axiom", "B");
	const string *rules = params.FindString("rules",&nr);
	int steps = params.FindOneInt("steps", 1);
	float delta = params.FindOneFloat("delta", 60);
	float cylinderRadius = params.FindOneFloat("cylinderRadius", 1.0);
	
	char key;
	string val;
	/* Rule is <char>-><string> which is key->val*/
	for(int i=0; i<nr;i++){
		key = rules[i][0];
		val = rules[i].substr(3,rules[i].length());

		rule_map[key].push_back(make_pair (val,0.33));
	}
	/* Calculate probabilities */
	size_t sz;
	for(RuleMap::iterator it = rule_map.begin(); it != rule_map.end(); ++it){
		/*cout << it->first << endl;*/
		sz = it->second.size();
		for(size_t s = 0; s < sz; s++){
			it->second[s].second = (float)(1.0/sz);
			/*cout << "\t" << it->second[s].first << " - " << it->second[s].second << endl;*/
		}
	}
	LSystem *s = new LSystem(o2w,w2o,reverseOrientation,axiom,rule_map,steps,delta);
	makePBRTSceneFile(s->generated_system, s->delta, cylinderRadius);
	return s;
}