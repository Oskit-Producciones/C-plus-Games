#include "glew.h"
#include "ParticleSystemManager.h"
#include "TextureManager.h"
#include <iostream>
using namespace std;

ParticleSystemManager* ParticleSystemManager::_instance = nullptr;

// constructor, no hace nada
ParticleSystemManager::ParticleSystemManager() {
	usePointSprites = false;
	
	//averiguamos si podemos usar pointSprites
	if(glewIsSupported("GL_ARB_point_sprite")) 
	{
		usePointSprites=true;
	}
	else{
		cerr<<"ERROR: la extension GL_ARB_point_sprite no esta disponible"<<endl;
	}	
	usePointSprites = true;
}

ParticleSystemManager::~ParticleSystemManager() {
	Clear();	
	ClearAffectors();
}

void ParticleSystemManager::Clear()
{		
	vector<ParticleSystem *>::iterator ps=particlesystems.begin();
	while(ps!=particlesystems.end()){
		delete *ps;		
		ps=particlesystems.erase(ps);		
	}
}

// crea un emisor y un nuevo sistema de particulas
// y nos devuelve una referencia a al emisor
Emitter &ParticleSystemManager::AddParticleSystem(unsigned nMaxParticles){
	Emitter *e=new Emitter;
	particlesystems.insert(particlesystems.end(),
							new ParticleSystem(*e, nMaxParticles));
	return *e;
}

// mueve las particulas de todos los sistemas
void ParticleSystemManager::Simulate(float dt){	
	vector<ParticleSystem *>::iterator ps=particlesystems.begin();

	ParticleSystem *ptemp;
	// mueve todos los sistemas
	while(ps!=particlesystems.end()){
		ptemp = (*(ps));
		ptemp->Move(dt);
		if(ptemp->nParticles==0 && ptemp->emitter->kill)
		{	
			ps=particlesystems.erase(ps);								
			delete ptemp;			
		}
		else
		{
			ps++;
		}
	}
}

// dibuja las particulas de todos los sistemas
void ParticleSystemManager::Render(sf::RenderWindow &w)
{
	if(usePointSprites) 
	{
		Render_PointSprites(w);
	}
	else
	{
		vector<ParticleSystem *>::iterator ps=particlesystems.begin();
		while(ps!=particlesystems.end()){
			ParticleSystem::Particle *p=(*ps)->particles;
			for(unsigned i=0; i<(*ps)->nMaxParticles; i++, p++){
				if(p->life>=0){
					w.Draw(*p);
				}
			}
			ps++;
		}
	}
}

void ParticleSystemManager::Render_PointSprites(sf::RenderWindow &w){
	vector<ParticleSystem *>::iterator ps=particlesystems.begin();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_POINT_SPRITE);	
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);	 
	while(ps!=particlesystems.end()){
		ParticleSystem::Particle *p=(*ps)->particles;
		glPointSize(p->GetSize().x * 2.0f);
		(*ps)->emitter->GetImage()->Bind();		
		glBegin(GL_POINTS);
		sf::Vector2f particlePosition;
		const Color *color = nullptr;
		for(unsigned i=0; i<(*ps)->nMaxParticles; i++, p++){
			if(p->life>=0){
				color = &p->GetColor();
				glColor4f(color->r,color->g,color->b,color->a/255.0f);
				particlePosition=p->GetPosition();
				glVertex2f(particlePosition.x, particlePosition.y);
			}
		}
		ps++;
		glEnd();
	}
	glDisable(GL_POINT_SPRITE);
}

unsigned ParticleSystemManager::GetNumParticles(){
	unsigned numParticles=0;
	vector<ParticleSystem *>::iterator ps=particlesystems.begin();
	while(ps!=particlesystems.end()){
		numParticles+=(*ps)->GetNumParticles();
		ps++;
	}
	return numParticles;
}

void ParticleSystemManager::AddAffector(Affector *a)
{
	affectors.push_back(a);
}

void ParticleSystemManager::ClearAffectors()
{
	vector<Affector *>::iterator it = affectors.begin();	
	while(it != affectors.end())
	{
		delete *it;				
		it = affectors.erase(it);
	}	
}

void ParticleSystemManager::CreateEmiterOneShoot(float x, float y)
{
	Emitter &e = AddParticleSystem(5);
	e.Spawn(false);
	e.SetImage(&TextureManager::GetInstance().GetTexture("../data/images/particula.png"));
	e.SetEmmitVel(200,200);
	e.SetEmmitLife(0.5f,0.0f);
	e.SetBlendMode(sf::Blend::Add);
	e.SetSpawnRate(100);

	vector<Affector *>::iterator it;

	for(it = affectors.begin();it != affectors.end();it++)
	{
		e.AddAffector(**it);
	}
	
	e.isOneTime = true;	
	e.SetPosition(x,y);
	e.Spawn(true);		
	e.Kill();
}

void ParticleSystemManager::CreateEmiterOneExplosion(float x, float y,sf::Color color)
{
	Emitter &e = AddParticleSystem(10);
	e.Spawn(false);
	e.SetImage(&TextureManager::GetInstance().GetTexture("../data/images/particula.png"));
	e.SetEmmitVel(200,50);
	e.SetEmmitLife(0.7f,0.7f);
	e.SetBlendMode(sf::Blend::Add);
	e.SetSpawnRate(100);
	e.SetColor(color);
	
	vector<Affector *>::iterator it;
	for(it = affectors.begin();it!= affectors.end();it++)
	{
		e.AddAffector(**it);
	}
	
	e.isOneTime = true;	
	e.SetPosition(x,y);
	e.Spawn(true);		
	e.Kill();
}