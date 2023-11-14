#include "ParticleShaderStruct.h"

#include <memory>
#include <vector>
#include "WorldTransform.h"
#include "Vector4.h"

class Particle {
	struct ParticleWorldTransform {
		ParticleMotion motion;
		Vector3 scale;
		Vector3 rotate;
		Vector3 transform;
		ParticleForGPU constantDate;
		void UpdateMatrix();
	};
public:
	~Particle();
	void Initialize(Emitter* emitter ,ParticleMotion* particleMotion);
	void Update();

	ParticleForGPU GetParticleForGPU(size_t num) { return particleWorldTransform_.at(num)->constantDate; }
	uint32_t GetAliveParticle() {return numAliveParticle_;}
	bool GetIsAlive() { return isAlive_; }
private:
	bool isAlive_;
	uint32_t numAliveParticle_;
	ParticleMotion* originalParticle_;
	Emitter* emitter_;
	std::vector<std::unique_ptr<ParticleWorldTransform>> particleWorldTransform_;
};