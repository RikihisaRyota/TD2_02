#include "ParticleShaderStruct.h"

#include <memory>
#include <vector>

#include "Random.h"
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
	void Reset();

	ParticleForGPU GetParticleForGPU(size_t num) { return particleWorldTransform_.at(num)->constantDate; }
	uint32_t GetAliveParticle() {return numAliveParticle_;}
	bool GetIsAlive() { return isAlive_; }
private:
	static bool CompareParticles(const std::unique_ptr<ParticleWorldTransform>& a, const std::unique_ptr<ParticleWorldTransform>& b) {
		return a->motion.isAlive > b->motion.isAlive;
	}
	Random::RandomNumberGenerator rnd_;
	bool isAlive_;
	uint32_t numAliveParticle_;
	int32_t flameInterval_;
	std::unique_ptr<ParticleMotion> originalParticle_;
	std::unique_ptr<Emitter> emitter_;
	std::vector<std::unique_ptr<ParticleWorldTransform>> particleWorldTransform_;
};