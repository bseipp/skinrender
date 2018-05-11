
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

#ifndef PBRT_MEDIA_GRID_H
#define PBRT_MEDIA_GRID_H

// media/grid.h*
#include "medium.h"
#include "transform.h"
#include "stats.h"
#include "floatfile.h"

namespace pbrt {

STAT_MEMORY_COUNTER("Memory/Volume density grid", densityBytes);

// GridDensityMedium Declarations
class GridDensityMedium : public Medium {
  public:
    // GridDensityMedium Public Methods
    GridDensityMedium(const Spectrum &sigma_a, const Spectrum &sigma_s, Float g,
                      int nx, int ny, int nz, const Transform &mediumToWorld,
                      const Float *d)
        : sigma_a(sigma_a),
          sigma_s(sigma_s),
          g(g),
          nx(nx),
          ny(ny),
          nz(nz),
          WorldToMedium(Inverse(mediumToWorld)),
          density(new Float[nx * ny * nz]) {
        densityBytes += nx * ny * nz * sizeof(Float);
        memcpy((Float *)density.get(), d, sizeof(Float) * nx * ny * nz);
        // Precompute values for Monte Carlo sampling of _GridDensityMedium_
        sigma_t = (sigma_a + sigma_s)[0];
        if (Spectrum(sigma_t) != sigma_a + sigma_s)
            Error(
                "GridDensityMedium requires a spectrally uniform attenuation "
                "coefficient!");
        Float maxDensity = 0;
        for (int i = 0; i < nx * ny * nz; ++i)
            maxDensity = std::max(maxDensity, density[i]);
        invMaxDensity = 1 / maxDensity;
    }
    // new constructor for Skin Grid
    GridDensityMedium(const Spectrum &sigma_a, const Spectrum &sigma_s, Float g,
                      int nx, int ny, int nz, const Transform &mediumToWorld,
                      const Float *d, const Float *tr, const Float *tg,
		       const Float *tb, const Float *sr, const Float *sg,
		       const Float *sb)
        : sigma_a(sigma_a),
          sigma_s(sigma_s),
          g(g),
          nx(nx),
          ny(ny),
          nz(nz),
          WorldToMedium(Inverse(mediumToWorld)),
          density(new Float[nx * ny * nz]),
	    trans_r(new Float[nx*ny*nz]),
	    trans_g(new Float[nx*ny*nz]),
	    trans_b(new Float[nx*ny*nz]),
	    scat_r(new Float[nx*ny*nz]),
	    scat_g(new Float[nx*ny*nz]),
	    scat_b(new Float[nx*ny*nz]){
        densityBytes += nx * ny * nz * sizeof(Float);
        densityBytes += nx * ny * nz * sizeof(Float);
        densityBytes += nx * ny * nz * sizeof(Float);
        densityBytes += nx * ny * nz * sizeof(Float);
        densityBytes += nx * ny * nz * sizeof(Float);
        densityBytes += nx * ny * nz * sizeof(Float);
        densityBytes += nx * ny * nz * sizeof(Float);
        memcpy((Float *)density.get(), d, sizeof(Float) * nx * ny * nz);
	memcpy((Float *)trans_r.get(), tr, sizeof(Float) * nx * ny * nz);
	//
	
    FILE *f = fopen("/home/aditya/development/ra/pbrt/trials/tests/pin_cushion/pbrt-v3/build/test.txt", "w+");
    if (!f) {
        Error("Unable to open file /home/aditya/development/ra/pbrt/trials/tests/pin_cushion/pbrt-v3/build/test.txt");
    }
    fclose(f);
	//
	//memcpy((Float *)trans_r.get(), tr, sizeof(Float));
	memcpy((Float *)trans_g.get(), tg, sizeof(Float) * nx * ny * nz);
        //memcpy((Float *)trans_g.get(), tg, sizeof(Float) * 1);
	memcpy((Float *)trans_b.get(), tb, sizeof(Float) * nx * ny * nz);
        //memcpy((Float *)trans_b.get(), tb, sizeof(Float) * 1);
	memcpy((Float *)scat_r.get(), sr, sizeof(Float) * nx * ny * nz);
        //memcpy((Float *)scat_r.get(), sr, sizeof(Float) * 1);
	memcpy((Float *)scat_g.get(), sg, sizeof(Float) * nx * ny * nz);
        //memcpy((Float *)scat_g.get(), sg, sizeof(Float) * 1);
	memcpy((Float *)scat_b.get(), sb, sizeof(Float) * nx * ny * nz);
        //memcpy((Float *)scat_b.get(), sb, sizeof(Float) * 1);
        // Precompute values for Monte Carlo sampling of _GridDensityMedium_
        sigma_t = (sigma_a + sigma_s)[0];
        if (Spectrum(sigma_t) != sigma_a + sigma_s)
            Error(
                "GridDensityMedium requires a spectrally uniform attenuation "
                "coefficient!");
        Float maxDensity = 0;
        for (int i = 0; i < nx * ny * nz; ++i)
            maxDensity = std::max(maxDensity, density[i]);
        invMaxDensity = 1 / maxDensity;
    }

    Float Density(const Point3f &p) const;
    Float TransmittancePropertyR(const Point3f &p) const;
    Float TransmittancePropertyG(const Point3f &p) const;
    Float TransmittancePropertyB(const Point3f &p) const;
    Float ScatteringProperty(const Point3f &p) const;
    Float D(const Point3i &p) const {
        Bounds3i sampleBounds(Point3i(0, 0, 0), Point3i(nx, ny, nz));
        if (!InsideExclusive(p, sampleBounds)) return 0;
        return density[(p.z * ny + p.y) * nx + p.x];
    }
    Float GetTransR(int index) const {
	return trans_r[index];
    }
    Float GetTransG(int index) const {
	return trans_g[index];
    }
    Float GetTransB(int index) const {
	return trans_b[index];
    }
    Float GetTransmittanceR(const Point3i &p) const {
	Bounds3i sampleBounds(Point3i(0, 0, 0), Point3i(nx, ny, nz));
	if (!InsideExclusive(p, sampleBounds)) return 0;
	return trans_r[(p.z * ny + p.y) * nx + p.x];
	//return trans_r[p.x*nx];
    }
    Float GetTransmittanceG(const Point3i &p) const {
	Bounds3i sampleBounds(Point3i(0, 0, 0), Point3i(nx, ny, nz));
	if (!InsideExclusive(p, sampleBounds)) return 0;
	return trans_g[(p.z * ny + p.y) * nx + p.x];
	//return trans_g[p.y*ny];
    }
    Float GetTransmittanceB(const Point3i &p) const {
	Bounds3i sampleBounds(Point3i(0, 0, 0), Point3i(nx, ny, nz));
	if (!InsideExclusive(p, sampleBounds)) return 0;
	return trans_b[(p.z * ny + p.y) * nx + p.x];
	//return trans_b[p.z*nz];
    }
    Float GetScatteringR(const Point3i &p) const {
        Bounds3i sampleBounds(Point3i(0, 0, 0), Point3i(nx, ny, nz));
        if (!InsideExclusive(p, sampleBounds)) return 0;
        return scat_r[(p.z * ny + p.y) * nx + p.x];
    }
    Float GetScatteringG(const Point3i &p) const {
        Bounds3i sampleBounds(Point3i(0, 0, 0), Point3i(nx, ny, nz));
        if (!InsideExclusive(p, sampleBounds)) return 0;
        return scat_g[(p.z * ny + p.y) * nx + p.x];
    }
    Float GetScatteringB(const Point3i &p) const {
        Bounds3i sampleBounds(Point3i(0, 0, 0), Point3i(nx, ny, nz));
        if (!InsideExclusive(p, sampleBounds)) return 0;
        return scat_b[(p.z * ny + p.y) * nx + p.x];
    }
    Spectrum Sample(const Ray &ray, Sampler &sampler, MemoryArena &arena,
                    MediumInteraction *mi) const;
    Spectrum Tr(const Ray &ray, Sampler &sampler) const;
    Spectrum Tr_Updated(const Ray &ray, Sampler &sampler) const;

  private:
    // GridDensityMedium Private Data
    const Spectrum sigma_a, sigma_s;
    const Float g;
    const int nx, ny, nz;
    const Transform WorldToMedium;
    std::unique_ptr<Float[]> density;
    std::unique_ptr<Float[]> trans_r, trans_g, trans_b, scat_r, scat_g, scat_b;
    Float sigma_t;
    Float invMaxDensity;
};

}  // namespace pbrt

#endif  // PBRT_MEDIA_GRID_H
