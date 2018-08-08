/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */


// materials/skin.cpp*
#include "materials/skin.h"
#include "textures/constant.h"
#include "spectrum.h"
#include "texture.h"
#include "paramset.h"
#include "interaction.h"
#include "scene.h"
#include "media/grid.h"
#include "interpolation.h"

namespace pbrt {

Float aer,aeg,aeb;
Float asr,asg,asb;

class SkinBSSRDF : public SeparableBSSRDF {
  public:
    Spectrum Sr(Float d) const{}
    Spectrum RenderSurface(Float r, Spectrum beta) const{
	Spectrum Sr(0.f);
	for (int ch = 0; ch < Spectrum::nSamples; ++ch) {
	    Float rOptical = r * sigma_t[ch];

	    int rhoOffset, radiusOffset;
	    Float rhoWeights[4], radiusWeights[4];
	    //if (!CatmullRomWeights(table.nRhoSamples, table.rhoSamples.get(),
				   //rho[ch], &rhoOffset, rhoWeights) ||
		//!CatmullRomWeights(table.nRadiusSamples, table.radiusSamples.get(),
				   //rOptical, &radiusOffset, radiusWeights))
	    if (!CatmullRomWeights(table.nRhoSamples, table.rhoSamples.get(),
				   rho[ch], &rhoOffset, rhoWeights))
		continue;

	    Float sr = 0;
	    for (int i = 0; i < 4; ++i) {
		//for (int j = 0; j < 4; ++j) {
		    Float weight = beta[ch];
		    //Float weight = rhoWeights[i] * radiusWeights[j];
		    if (weight != 0)
			sr += weight *
			      table.profile[rhoOffset + i];
			      //table.EvalProfile(rhoOffset + i, radiusOffset + j);
		//}
	    }

	    //if (rOptical != 0) sr /= 2 * Pi * rOptical;
	    Sr[ch] = sr;
	}
	Sr *= sigma_t * sigma_t;
	return Sr.Clamp();
    };
    Float Sample_Sr(int ch, Float u) const{
    };
    Float Pdf_Sr(int ch, Float r) const{
    };
    Spectrum Sample_S_Updated(const Scene &scene, Float u1, const Point2f &u2,
                      MemoryArena &arena, SurfaceInteraction *si,
                      Float *pdf, Ray currentRay, Vector3f wi,
		      Sampler &sampler) const;

    SkinBSSRDF(const SurfaceInteraction &po, const Material *material,
                    TransportMode mode, Float eta, const Spectrum &sigma_a,
                    const Spectrum &sigma_s, const BSSRDFTable &table,
		    Float addition_param, Float variant_param,
		    Ray &ray, Sampler &sampler)
        : SeparableBSSRDF(po, eta, material, mode, variant_param), table(table),
	    addition_param(addition_param),
	    ray(ray), sampler(sampler), sigma_s(sigma_s),
	    sigma_a(sigma_a) {
		sigma_t = sigma_s + sigma_a;
		for (int c = 0; c < Spectrum::nSamples; ++c)
		    rho[c] = sigma_t[c] != 0 ? (sigma_s[c] / sigma_t[c]) : 0;
	    }

    Float Pdf_Sp(const SurfaceInteraction &si) const;

    Spectrum Generate_surin(const Scene &scene,
	    SurfaceInteraction *pi,
	    MemoryArena &arena, Sampler &sampler,
	    const Point2f &u2, Float u1, Float *pdf,
	    Ray ray) const;

  private:
    // TabulatedBSSRDF Private Data
    const BSSRDFTable &table;
    Spectrum sigma_t, rho, sigma_s, sigma_a;
    Float addition_param;
    Float variant_param;
    Ray &ray;
    Sampler &sampler;
};

Float SkinBSSRDF::Pdf_Sp(const SurfaceInteraction &pi)const {
    Vector3f d = po.p - pi.p;
    //Vector3f dLocal(Dot(ss, d), Dot(ts, d), Dot(ns, d));
    Normal3f nLocal(Dot(ss, pi.n), Dot(ts, pi.n), Dot(ns, pi.n));

    // Compute BSSRDF profile radius under projection along each axis
    //Float rProj[3] = {std::sqrt(dLocal.y * dLocal.y + dLocal.z * dLocal.z),
                      //std::sqrt(dLocal.z * dLocal.z + dLocal.x * dLocal.x),
                      //std::sqrt(dLocal.x * dLocal.x + dLocal.y * dLocal.y)};

    // Return combined probability from all BSSRDF sampling strategies
    Float pdf = 0, axisProb[3] = {.25f, .25f, .5f};
    Float chProb = 1 / (Float)Spectrum::nSamples;
    for (int axis = 0; axis < 3; ++axis)
        for (int ch = 0; ch < Spectrum::nSamples; ++ch)
            pdf += std::abs(nLocal[axis]) * chProb *
                   axisProb[axis];
    return pdf;
}

Spectrum SkinBSSRDF::Sample_S_Updated(const Scene &scene, Float u1, const Point2f &u2,
                      MemoryArena &arena, SurfaceInteraction *si,
                      Float *pdf, Ray ray, Vector3f wi,
		      Sampler &sampler) const {

    // test code starts
    //
    Spectrum Sp = Generate_surin(scene, si, arena, sampler, u2, u1, pdf,
		    ray);
    if (!Sp.IsBlack()) {
	si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
	si->bsdf->Add(ARENA_ALLOC(arena, SeparableBSSRDFAdapter)(this));
	si->wo = Vector3f(si->shading.n);
    }
    return Sp;
}

Spectrum SkinBSSRDF::Generate_surin(const Scene &scene,
	SurfaceInteraction *pi,
	MemoryArena &arena, Sampler &sampler,
	const Point2f &u2, Float u1, Float *pdf,
	Ray old_ray) const {
    Spectrum beta = Spectrum(1.f);
    MediumInteraction mi;
    int mis = 0;
    Vector3f wo;
    SurfaceInteraction isect;
    bool intersected_inside = false;
    Ray prev_ray = old_ray;
    Ray ray = old_ray;
    while(true){
	if (ray.medium) ray.medium->Sample(ray, sampler, arena, &mi);
	if(mi.IsValid() && mis < 25){
	    ++mis;
	    wo = -ray.d;
	    Vector3f wi;
	    mi.phase->Sample_p(wo, &wi, u2);
	    ray = mi.SpawnRay(wi);
	    if(scene.Intersect(ray, &isect)){
		intersected_inside = true;
		break;
	    }
	    else{
		prev_ray = ray;
		beta *= ray.medium->Tr(ray, sampler);
		//beta = ray.medium->Tr_Updated(ray, sampler);
		continue;
	    }
	}
	if(intersected_inside || scene.Intersect(ray, &isect)){
	    if(isect.primitive->GetMaterial() == this->material) {
		if(!intersected_inside){
		}
		break;
	    }
	}
	return Spectrum(0.f);
    }
    *pi = isect;
    *pdf = 0.47;
    //*pdf = this->Pdf_Sp(*pi) / mis;
    Float ptr, ptg, ptb;
    Float vox_r, vox_g, vox_b;
    Point3f voxel_cords(ray.o.x * 100 - .5f, ray.o.y * 100 - .5f, ray.o.z * 40 - .5f);
    Point3i voxel_locs = (Point3i)Floor(voxel_cords);
    if(voxel_locs[0] < 0)
	voxel_locs[0] *= -1;
    if(voxel_locs[1] < 0)
	voxel_locs[1] *= -1;
    if(voxel_locs[2] < 0)
	voxel_locs[2] *= -1;
    //printf("\nusing :: %d, %d,%d\n", (int)ray.medium->GetTransR(voxel_locs[0]), (int)ray.medium->GetTransR(voxel_locs[1]), (int)ray.medium->GetTransR(voxel_locs[2]));
    vox_r = this->material->GetTransmittanceRGB((int)ray.medium->GetTransR(voxel_locs[0]));
    vox_g = this->material->GetTransmittanceRGB((int)ray.medium->GetTransR(voxel_locs[1]));
    vox_b = this->material->GetTransmittanceRGB((int)ray.medium->GetTransR(voxel_locs[2]));
    //printf("\nusing :: %f, %f,%f\n", vox_r, vox_g, vox_b);
    Float rgbarray[3] = {vox_r, vox_g,vox_b};
    Spectrum scat_albedo = Spectrum::FromRGB(rgbarray,SpectrumType::Illuminant);
    beta = scat_albedo.divideMe(beta, Spectrum::nSamples);
    return beta.Clamp();
}

// SkinMaterial Method Definitions
void SkinMaterial::ComputeScatteringFunctionsUpdated(
    SurfaceInteraction *si, MemoryArena &arena, TransportMode mode,
    bool allowMultipleLobes, Ray &ray,
    Sampler &sampler) const {
    // Perform bump mapping with _bumpMap_, if present
    if (bumpMap) Bump(bumpMap, si);
    Spectrum R = Kr->Evaluate(*si).Clamp();
    Spectrum T = Kt->Evaluate(*si).Clamp();
    Float urough = uRoughness->Evaluate(*si);
    Float vrough = vRoughness->Evaluate(*si);

    // Initialize _bsdf_ for smooth or rough dielectric
    si->bsdf = ARENA_ALLOC(arena, BSDF)(*si, eta);

    if (R.IsBlack() && T.IsBlack()) return;

    bool isSpecular = urough == 0 && vrough == 0;
    if (isSpecular && allowMultipleLobes) {
        si->bsdf->Add(
            ARENA_ALLOC(arena, FresnelSpecular)(R, T, 1.f, eta, mode));
    } else {
        if (remapRoughness) {
            urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
            vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
        }
        MicrofacetDistribution *distrib =
            isSpecular ? nullptr
                       : ARENA_ALLOC(arena, TrowbridgeReitzDistribution)(
                             urough, vrough);
        if (!R.IsBlack()) {
            Fresnel *fresnel = ARENA_ALLOC(arena, FresnelDielectric)(1.f, eta);
            if (isSpecular)
                si->bsdf->Add(
                    ARENA_ALLOC(arena, SpecularReflection)(R, fresnel));
            else
                si->bsdf->Add(ARENA_ALLOC(arena, MicrofacetReflection)(
                    R, distrib, fresnel));
        }
        if (!T.IsBlack()) {
            if (isSpecular)
                si->bsdf->Add(ARENA_ALLOC(arena, SpecularTransmission)(
                    T, 1.f, eta, mode));
            else
                si->bsdf->Add(ARENA_ALLOC(arena, MicrofacetTransmission)(
                    T, distrib, 1.f, eta, mode));
        }
    }

    Spectrum mfree = scale * mfp->Evaluate(*si).Clamp();
    Spectrum kd = Kd->Evaluate(*si).Clamp();
    Spectrum sig_a, sig_s;
    SubsurfaceFromDiffuse(table, kd, mfree, &sig_a, &sig_s);
    si->bssrdf = ARENA_ALLOC(arena, SkinBSSRDF)(*si, this, mode, eta,
                                                     sig_a, sig_s, table,
						     this->addition_param,
						     this->variant_param,
						     ray, sampler);
}

SkinMaterial *CreateSkinMaterial(const TextureParams &mp) {
    Float Kd[3] = {.5, .5, .5};
    std::shared_ptr<Texture<Spectrum>> kd =
        mp.GetSpectrumTexture("Kd", Spectrum::FromRGB(Kd));
    std::shared_ptr<Texture<Spectrum>> mfp =
        mp.GetSpectrumTexture("mfp", Spectrum(1.f));
    std::shared_ptr<Texture<Spectrum>> kr =
        mp.GetSpectrumTexture("Kr", Spectrum(1.f));
    std::shared_ptr<Texture<Spectrum>> kt =
        mp.GetSpectrumTexture("Kt", Spectrum(1.f));
    std::shared_ptr<Texture<Float>> roughu =
        mp.GetFloatTexture("uroughness", 0.f);
    std::shared_ptr<Texture<Float>> roughv =
        mp.GetFloatTexture("vroughness", 0.f);
    std::shared_ptr<Texture<Float>> bumpMap =
        mp.GetFloatTextureOrNull("bumpmap");
    Float eta = mp.FindFloat("eta", 1.33f);
    Float scale = mp.FindFloat("scale", 1.0f);
    Float addition_param = mp.FindFloat("addpara", 0.00008f);
    Float variant_param = mp.FindFloat("vartpara", 0.0f);
    Float g = mp.FindFloat("g", 0.0f);
    aer = mp.FindFloat("aer", 0.0f);
    aeg = mp.FindFloat("aeg", 0.0f);
    aeb = mp.FindFloat("aeb", 0.0f);
    asr = mp.FindFloat("asr", 0.0f);
    asg = mp.FindFloat("asg", 0.0f);
    asb = mp.FindFloat("asb", 0.0f);
    std::string t1 = mp.FindString("t1");
    std::vector<Float> tissue_one;
    std::vector<Float> tissue_two;
    std::vector<Float> tissue_three;
    std::vector<Float> tissue_four;
    std::vector<Float> tissue_five;
    char curNumber[32];
    int float_index = 0, pos = 0;
    int tracked_floats = 0, tissue_index = 0;
    for(unsigned int i = 0; i< t1.length(); i++)
    {
	if(isspace(t1[i]))
	{
	    curNumber[pos++] = '\0';
	    //printf("\n%f\n", atof(curNumber));
	    switch(tissue_index) {
	    case 0:
		tissue_one.push_back(atof(curNumber));
	    case 1:
		tissue_two.push_back(atof(curNumber));
	    case 2:
		tissue_three.push_back(atof(curNumber));
	    case 3:
		tissue_four.push_back(atof(curNumber));
	    case 4:
		tissue_five.push_back(atof(curNumber));
	    }
	    tracked_floats++;
	    if(tracked_floats == 5) {
		tissue_index++;
		tracked_floats = 0;
		float_index = 0;
	    }
	    pos = 0;
	}
	else
	    curNumber[pos++] = t1[i];
    }
    if (pos > 0)
    {
	curNumber[pos++] = '\0';
	    switch(tissue_index) {
	    case 0:
		tissue_one.push_back(atof(curNumber));
	    case 1:
		tissue_two.push_back(atof(curNumber));
	    case 2:
		tissue_three.push_back(atof(curNumber));
	    case 3:
		tissue_four.push_back(atof(curNumber));
	    case 4:
		tissue_five.push_back(atof(curNumber));
	    }
    }
    //printf("\n%f, %f, %f, %f, %f, %f\n", tissue_one[0], tissue_one[1], tissue_one[2], tissue_one[3], tissue_one[4], tissue_one[5]);
    //printf("\n%f, %f, %f, %f, %f, %f\n", tissue_two[0], tissue_two[1], tissue_two[2], tissue_two[3], tissue_two[4], tissue_two[5]);
    const Float *ts1 = &tissue_one[0];
    const Float *t2 = &tissue_two[0];
    const Float *t3 = &tissue_three[0];
    const Float *t4 = &tissue_four[0];
    const Float *t5 = &tissue_five[0];
    //tissues_pointer = &tissues[0];
    bool remapRoughness = mp.FindBool("remaproughness", true);
    return new SkinMaterial(scale, kd, kr, kt, mfp, g, eta, roughu,
                                    roughv, bumpMap, remapRoughness,
				    addition_param,
				    variant_param,
				    ts1,
				    t2,
				    t3,
				    t4,
				    t5);
}

}  // namespace pbrt
