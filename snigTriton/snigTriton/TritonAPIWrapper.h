#ifndef BYY_TRITON_API_WRAPPER
#define BYY_TRITON_API_WRAPPER 1

#include "TritonExport.h"
#include <osg/Vec3>
#include <stdint.h> // for uintptr_t

/** Enumerates the ocean quality settings used in Ocean::SetQuality() */
enum OceanQuality {
    GOOD,
    BETTER,
    BEST
};

class SNIGRITON_EXPORT BreakingWavesParameters
{
public:
    void SetSteepness(float);
    float GetSteepness() const;

    void SetWavelength(float);
    float GetWavelength() const;

public:
    BreakingWavesParameters(uintptr_t handle) : _handle(handle) { }
    uintptr_t _handle;
};

class SNIGRITON_EXPORT Environment
{
public:
    void SetDirectionalLight(const osg::Vec3& dir, const osg::Vec3& color);
    osg::Vec3 GetLightDirection() const;
    osg::Vec3 GetDirectionalLightColor() const;

    void SetAmbientLight(const osg::Vec3& color);
    osg::Vec3 GetAmbientLightColor() const;

    void SetSunIntensity(float intensity);
    float GetSunIntensity() const;

    BreakingWavesParameters GetBreakingWavesParameters() const;

    void SimulateSeaState(double bs, double winddir);

public:
    Environment(uintptr_t handle) : _handle(handle) { }
    uintptr_t _handle;
};

class SNIGRITON_EXPORT Ocean
{
public:
    void SetChoppiness(float);
    float GetChoppiness() const;

    void EnableWireframe(bool wireframe);

    void SetQuality(OceanQuality value);
    OceanQuality GetQuality() const;

    //void SetMaximumWavePeriod(double value);
    //double GetMaximumWavePeriod() const;

    void EnableSpray(bool enabled);
    bool SprayEnabled() const;

    void EnableGodRays(bool enabled);
    bool GodRaysEnabled() const;

    void SetGodRaysFade(float fadeAmount);
    float GetGodRaysFade() const;

public:
    Ocean(uintptr_t handle) : _handle(handle) { }
    uintptr_t _handle;
};

#endif // BYY_TRITON_API_WRAPPER
