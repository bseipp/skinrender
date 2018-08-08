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

#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_MATERIALS_SKIN_H
#define PBRT_MATERIALS_SKIN_H

// materials/kdsubsurface.h*
#include "pbrt.h"
#include "reflection.h"
#include "material.h"
#include "bssrdf.h"

namespace pbrt {

// SkinMaterial Declarations
class SkinMaterial : public Material {
  public:
    // SkinMaterial Public Methods
    SkinMaterial(Float scale,
                         const std::shared_ptr<Texture<Spectrum>> &Kd,
                         const std::shared_ptr<Texture<Spectrum>> &Kr,
                         const std::shared_ptr<Texture<Spectrum>> &Kt,
                         const std::shared_ptr<Texture<Spectrum>> &mfp, Float g,
                         Float eta,
                         const std::shared_ptr<Texture<Float>> &uRoughness,
                         const std::shared_ptr<Texture<Float>> &vRoughness,
                         const std::shared_ptr<Texture<Float>> &bumpMap,
                         bool remapRoughness,
			 Float addition_param,
			 Float variant_param,
			 const Float *ts1,
			 const Float *ts2,
			 const Float *ts3,
			 const Float *ts4,
			 const Float *ts5)
        : scale(scale),
          Kd(Kd),
          Kr(Kr),
          Kt(Kt),
          mfp(mfp),
          uRoughness(uRoughness),
          vRoughness(vRoughness),
          bumpMap(bumpMap),
          eta(eta),
          remapRoughness(remapRoughness),
          table(100, 64),
	  addition_param(addition_param),
	  variant_param(variant_param),
	    tissue_one(new Float[6]),
	    tissue_two(new Float[6]),
	    tissue_three(new Float[6]),
	    tissue_four(new Float[6]),
	    tissue_five(new Float[6]) {
	      this->is_skin_material = true;
	    memcpy((Float *)tissue_one.get(), ts1, sizeof(Float) * 6);
	    memcpy((Float *)tissue_two.get(), ts2, sizeof(Float) * 6);
	    memcpy((Float *)tissue_three.get(), ts3, sizeof(Float) * 6);
	    memcpy((Float *)tissue_four.get(), ts4, sizeof(Float) * 6);
	    memcpy((Float *)tissue_five.get(), ts5, sizeof(Float) * 6);
	    ComputeBeamDiffusionBSSRDF(g, eta, &table);
    }
    void ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena,
                                    TransportMode mode,
                                    bool allowMultipleLobes) const
    {
    };
    void ComputeScatteringFunctionsUpdated(SurfaceInteraction *si, MemoryArena &arena,
                                    TransportMode mode,
                                    bool allowMultipleLobes,
				    Ray &ray,
				    Sampler &sampler) const;
    Float GetTransmittanceRGB(int tissue_index, int type) const {
	    switch(char(tissue_index)) {
	    case '0':
		printf("\n%c, %f\n", char(tissue_index), tissue_one[type]);
		return tissue_one[type];
	    case '1':
		printf("\n%c, %f\n", char(tissue_index), tissue_two[type]);
		return tissue_two[type];
	    case '2':
		printf("\n%c, %f\n", char(tissue_index), tissue_three[type]);
		return tissue_three[type];
	    case '3':
		printf("\n%c, %f\n", char(tissue_index), tissue_four[type]);
		return tissue_four[type];
	    case '4':
		printf("\n%c, %f\n", char(tissue_index), tissue_four[type]);
		return tissue_five[type];
	    case '5':
		printf("\n%c, %f\n", char(tissue_index), tissue_five[type]);
		return tissue_five[type];
	    }
	    printf("\nNaN\n");
	    return 0.f;
    }
  private:
    // SkinMaterial Private Data
    Float scale;
    std::shared_ptr<Texture<Spectrum>> Kd, Kr, Kt, mfp;
    std::shared_ptr<Texture<Float>> uRoughness, vRoughness;
    std::shared_ptr<Texture<Float>> bumpMap;
    Float eta;
    bool remapRoughness;
    BSSRDFTable table;
    Float addition_param;
    Float variant_param;
    std::unique_ptr<Float[]> tissue_one;
    std::unique_ptr<Float[]> tissue_two;
    std::unique_ptr<Float[]> tissue_three;
    std::unique_ptr<Float[]> tissue_four;
    std::unique_ptr<Float[]> tissue_five;
};

SkinMaterial *CreateSkinMaterial(const TextureParams &mp);

}  // namespace pbrt

#endif  // PBRT_MATERIALS_KDSUBSURFACE_H
