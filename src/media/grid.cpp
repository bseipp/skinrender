
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


// media/grid.cpp*
#include "media/grid.h"
#include "paramset.h"
#include "sampler.h"
#include "stats.h"
#include "interaction.h"

namespace pbrt {

STAT_RATIO("Media/Grid steps per Tr() call", nTrSteps, nTrCalls);

// changes for skin start

// changes for skin end
Float GridDensityMedium::ScatteringProperty(const Point3f &p) const {
    // Compute voxel coordinates and offsets for _p_
    Point3f pSamples(p.x * nx - .5f, p.y * ny - .5f, p.z * nz - .5f);
    Point3i pi = (Point3i)Floor(pSamples);
    Vector3f d = pSamples - (Point3f)pi;

    // Trilinearly interpolate density values to compute local density
    //Float d00 = Lerp(d.x, GetScattering(pi), GetScattering(pi + Vector3i(1, 0, 0)));
    //Float d10 = Lerp(d.x, GetScattering(pi + Vector3i(0, 1, 0)), GetScattering(pi + Vector3i(1, 1, 0)));
    //Float d01 = Lerp(d.x, GetScattering(pi + Vector3i(0, 0, 1)), GetScattering(pi + Vector3i(1, 0, 1)));
    //Float d11 = Lerp(d.x, GetScattering(pi + Vector3i(0, 1, 1)), GetScattering(pi + Vector3i(1, 1, 1)));
    //Float d0 = Lerp(d.y, d00, d10);
    //Float d1 = Lerp(d.y, d01, d11);
    //return Lerp(d.z, d0, d1);
    return 0.f;
}
Float GridDensityMedium::TransmittancePropertyR(const Point3f &p) const {
    // Compute voxel coordinates and offsets for _p_
    Point3f pSamples(p.x * nx - .5f, p.y * ny - .5f, p.z * nz - .5f);
    Point3i pi = (Point3i)Floor(pSamples);
    Vector3f d = pSamples - (Point3f)pi;

    Float d00 = Lerp(d.x, GetTransmittanceR(pi), GetTransmittanceR(pi + Vector3i(1, 0, 0)));
    Float d10 = Lerp(d.x, GetTransmittanceR(pi + Vector3i(0, 1, 0)), GetTransmittanceR(pi + Vector3i(1, 1, 0)));
    Float d01 = Lerp(d.x, GetTransmittanceR(pi + Vector3i(0, 0, 1)), GetTransmittanceR(pi + Vector3i(1, 0, 1)));
    Float d11 = Lerp(d.x, GetTransmittanceR(pi + Vector3i(0, 1, 1)), GetTransmittanceR(pi + Vector3i(1, 1, 1)));
    Float d0 = Lerp(d.y, d00, d10);
    Float d1 = Lerp(d.y, d01, d11);
    return Lerp(d.z, d0, d1);
}

Float GridDensityMedium::TransmittancePropertyG(const Point3f &p) const {
    // Compute voxel coordinates and offsets for _p_
    Point3f pSamples(p.x * nx - .5f, p.y * ny - .5f, p.z * nz - .5f);
    Point3i pi = (Point3i)Floor(pSamples);
    Vector3f d = pSamples - (Point3f)pi;

    Float d00 = Lerp(d.x, GetTransmittanceG(pi), GetTransmittanceG(pi + Vector3i(1, 0, 0)));
    Float d10 = Lerp(d.x, GetTransmittanceG(pi + Vector3i(0, 1, 0)), GetTransmittanceG(pi + Vector3i(1, 1, 0)));
    Float d01 = Lerp(d.x, GetTransmittanceG(pi + Vector3i(0, 0, 1)), GetTransmittanceG(pi + Vector3i(1, 0, 1)));
    Float d11 = Lerp(d.x, GetTransmittanceG(pi + Vector3i(0, 1, 1)), GetTransmittanceG(pi + Vector3i(1, 1, 1)));
    Float d0 = Lerp(d.y, d00, d10);
    Float d1 = Lerp(d.y, d01, d11);
    return Lerp(d.z, d0, d1);
}

Float GridDensityMedium::TransmittancePropertyB(const Point3f &p) const {
    // Compute voxel coordinates and offsets for _p_
    Point3f pSamples(p.x * nx - .5f, p.y * ny - .5f, p.z * nz - .5f);
    Point3i pi = (Point3i)Floor(pSamples);
    Vector3f d = pSamples - (Point3f)pi;

    Float d00 = Lerp(d.x, GetTransmittanceB(pi), GetTransmittanceB(pi + Vector3i(1, 0, 0)));
    Float d10 = Lerp(d.x, GetTransmittanceB(pi + Vector3i(0, 1, 0)), GetTransmittanceB(pi + Vector3i(1, 1, 0)));
    Float d01 = Lerp(d.x, GetTransmittanceB(pi + Vector3i(0, 0, 1)), GetTransmittanceB(pi + Vector3i(1, 0, 1)));
    Float d11 = Lerp(d.x, GetTransmittanceB(pi + Vector3i(0, 1, 1)), GetTransmittanceB(pi + Vector3i(1, 1, 1)));
    Float d0 = Lerp(d.y, d00, d10);
    Float d1 = Lerp(d.y, d01, d11);
    return Lerp(d.z, d0, d1);
}

// GridDensityMedium Method Definitions
Float GridDensityMedium::Density(const Point3f &p) const {
    // Compute voxel coordinates and offsets for _p_
    Point3f pSamples(p.x * nx - .5f, p.y * ny - .5f, p.z * nz - .5f);
    Point3i pi = (Point3i)Floor(pSamples);
    Vector3f d = pSamples - (Point3f)pi;

    // Trilinearly interpolate density values to compute local density
    Float d00 = Lerp(d.x, D(pi), D(pi + Vector3i(1, 0, 0)));
    Float d10 = Lerp(d.x, D(pi + Vector3i(0, 1, 0)), D(pi + Vector3i(1, 1, 0)));
    Float d01 = Lerp(d.x, D(pi + Vector3i(0, 0, 1)), D(pi + Vector3i(1, 0, 1)));
    Float d11 = Lerp(d.x, D(pi + Vector3i(0, 1, 1)), D(pi + Vector3i(1, 1, 1)));
    Float d0 = Lerp(d.y, d00, d10);
    Float d1 = Lerp(d.y, d01, d11);
    return Lerp(d.z, d0, d1);
}

Spectrum GridDensityMedium::Sample(const Ray &rWorld, Sampler &sampler,
                                   MemoryArena &arena,
                                   MediumInteraction *mi) const {
    ProfilePhase _(Prof::MediumSample);
    Ray ray = WorldToMedium(
        Ray(rWorld.o, Normalize(rWorld.d), rWorld.tMax * rWorld.d.Length()));
    // Compute $[\tmin, \tmax]$ interval of _ray_'s overlap with medium bounds
    const Bounds3f b(Point3f(0, 0, 0), Point3f(1, 1, 1));
    Float tMin, tMax;
    if (!b.IntersectP(ray, &tMin, &tMax)) return Spectrum(1.f);

    // Run delta-tracking iterations to sample a medium interaction
    Float t = tMin;
    while (true) {
        t -= std::log(1 - sampler.Get1D()) * invMaxDensity / sigma_t;
        if (t >= tMax) break;
        if (Density(ray(t)) * invMaxDensity > sampler.Get1D()) {
            // Populate _mi_ with medium interaction information and return
            PhaseFunction *phase = ARENA_ALLOC(arena, HenyeyGreenstein)(g);
            *mi = MediumInteraction(rWorld(t), -rWorld.d, rWorld.time, this,
                                    phase);
            return sigma_s / sigma_t;
        }
    }
    return Spectrum(1.f);
}

Spectrum GridDensityMedium::Tr(const Ray &rWorld, Sampler &sampler) const {
    ProfilePhase _(Prof::MediumTr);
    ++nTrCalls;

    Ray ray = WorldToMedium(
        Ray(rWorld.o, Normalize(rWorld.d), rWorld.tMax * rWorld.d.Length()));
    // Compute $[\tmin, \tmax]$ interval of _ray_'s overlap with medium bounds
    const Bounds3f b(Point3f(0, 0, 0), Point3f(1, 1, 1));
    Float tMin, tMax;
    if (!b.IntersectP(ray, &tMin, &tMax)){
	return Spectrum(1.f);
    }

    // Perform ratio tracking to estimate the transmittance value
    Float Tr = 1, t = tMin;
    while (true) {
        ++nTrSteps;
	t -= std::log(1 - sampler.Get1D()) * invMaxDensity / sigma_t;
        if (t >= tMax) break;
        Float density = Density(ray(t));
        Tr *= 1 - std::max((Float)0, density * invMaxDensity);
        // Added after book publication: when transmittance gets low,
        // start applying Russian roulette to terminate sampling.
        const Float rrThreshold = .1;
        if (Tr < rrThreshold) {
            Float q = std::max((Float).05, 1 - Tr);
            if (sampler.Get1D() < q) return 0;
            Tr /= 1 - q;
        }
    }
    return Spectrum(Tr);
}

Spectrum GridDensityMedium::Tr_Updated(const Ray &rWorld, Sampler &sampler) const {
    ProfilePhase _(Prof::MediumTr);
    ++nTrCalls;

    Ray ray = WorldToMedium(
        Ray(rWorld.o, Normalize(rWorld.d), rWorld.tMax * rWorld.d.Length()));
    // Compute $[\tmin, \tmax]$ interval of _ray_'s overlap with medium bounds
    const Bounds3f b(Point3f(0, 0, 0), Point3f(1, 1, 1));
    Float tMin, tMax;
    if (!b.IntersectP(ray, &tMin, &tMax)){
	return Spectrum(1.f);
    }

    // Perform ratio tracking to estimate the transmittance value
    Float Tr = 1, t = tMin;
    Float scattering_at_point = 0, transmittance_at_point = 0;
    while (true) {
        ++nTrSteps;
	t -= std::log(1 - sampler.Get1D()) * invMaxDensity / sigma_t;
        if (t >= tMax) break;
        Float density = Density(ray(t));
        Tr *= 1 - std::max((Float)0, density * invMaxDensity);
        // Added after book publication: when transmittance gets low,
        // start applying Russian roulette to terminate sampling.
        const Float rrThreshold = .1;
        if (Tr < rrThreshold) {
            Float q = std::max((Float).05, 1 - Tr);
            if (sampler.Get1D() < q) return 0;
            Tr /= 1 - q;
        }
    }
    //scattering_at_point = ScatteringProperty(ray.o);
    //transmittance_at_point = TransmittanceProperty(ray.o);
    //Spectrum scattering_spectrum = Spectrum(scattering_at_point);
    //Spectrum transmittance_spectrum = Spectrum(transmittance_at_point);
    Point3f pSamples(ray.o.x * nx - .5f, ray.o.y * ny - .5f, ray.o.z * nz - .5f);
    Point3i pi = (Point3i)Floor(pSamples);
    //Float ptr = GetTransmittance(pi);
    //Float ptg = 25.f, ptb = 150.f;
    //if(ptr == 0.f) {
	//ptr = 234.f;
	//ptg = 150.f;
	//ptb = 48.f;
    //}
    //if(ptr > 200.f){
	//ptr = 134.f;
	//ptg = 70.f;
	//ptb = 98.f;
    //}
    //if(ptr > 100.f && ptr < 200.f){
	//ptr = 134.f;
	//ptg = 200.f;
	//ptb = 218.f;
    //}
    //if(ptr < 100.f){
	//ptr = 34.f;
	//ptg = 215.f;
	//ptb = 147.f;
    //}
    //printf("$%f$", ptr);
    //Float ptr = 185.f;
    //if(scattering_spectrum.IsBlack() && transmittance_spectrum.IsBlack())
	//return Spectrum(Tr);
    //ptr = 49.f;
    //ptg = 198.f;
    //ptb = 200.f;
    //Float rgbarray[3] = {ptr, ptg, ptb};
    //Spectrum beta = Spectrum(Tr);
    //Spectrum beta = Spectrum::FromRGB(rgbarray, SpectrumType::Illuminant);
    //return beta;
    //return beta.divideMe(Spectrum(Tr),
			//Spectrum::nSamples);
    //return beta.divideMe(Spectrum::FromRGB(rgbarray,
			    //SpectrumType::Illuminant),
			//Spectrum::nSamples);
    //return scattering_spectrum.divideMe(Spectrum(Tr), Spectrum::nSamples);
    return Spectrum(Tr);
}

}  // namespace pbrt
