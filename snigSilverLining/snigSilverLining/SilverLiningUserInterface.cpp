#include "SilverLiningUserInterface.h"
#include <CloudTypes.h>
#include <CloudLayer.h>
#include <CloudLayerFactory.h>

SilverLiningUserInterface::SilverLiningUserInterface(void)
	:myNewCloudLayerType(-2)
{
}


SilverLiningUserInterface::~SilverLiningUserInterface(void)
{
}

void SilverLiningUserInterface::onDrawSky()
{
	updateCloudLayer();
	updateFog();
}

void SilverLiningUserInterface::setCloudLayerType( int type )
{
	myNewCloudLayerType=type;
}

void SilverLiningUserInterface::updateCloudLayer()
{
	if (myNewCloudLayerType==-2)
		return;

	myAtmosphere->GetConditions()->RemoveAllCloudLayers();

	SilverLining::CloudLayer *cloudLayer;

	switch(myNewCloudLayerType)
	{
	case CIRROCUMULUS:
		break;
	case CIRRUS_FIBRATUS:
		cloudLayer = SilverLining::CloudLayerFactory::Create(CIRRUS_FIBRATUS);
		cloudLayer->SetBaseAltitude(6000);
		cloudLayer->SetThickness(0);
		cloudLayer->SetBaseLength(100000);
		cloudLayer->SetBaseWidth(100000);
		cloudLayer->SetLayerPosition(0, 0);
		cloudLayer->SeedClouds(*myAtmosphere);

		myAtmosphere->GetConditions()->AddCloudLayer(cloudLayer);
		break;
	case STRATUS :
		cloudLayer = SilverLining::CloudLayerFactory::Create(STRATUS);
		cloudLayer->SetIsInfinite(true);
		cloudLayer->SetBaseAltitude(1000);
		cloudLayer->SetThickness(600);
		cloudLayer->SetDensity(1.0);
		cloudLayer->SetLayerPosition(0, 0);
		cloudLayer->SeedClouds(*myAtmosphere);

		myAtmosphere->GetConditions()->AddCloudLayer(cloudLayer);
		break;
	case CUMULUS_MEDIOCRIS:
		cloudLayer = SilverLining::CloudLayerFactory::Create(CUMULUS_MEDIOCRIS);
		cloudLayer->SetIsInfinite(true);
		cloudLayer->SetBaseAltitude(1000);
		cloudLayer->SetThickness(100);
		cloudLayer->SetBaseLength(20000);
		cloudLayer->SetBaseWidth(20000);
		cloudLayer->SetDensity(0.2);
		cloudLayer->SetLayerPosition(0, 0);
		cloudLayer->SeedClouds(*myAtmosphere);

		myAtmosphere->GetConditions()->AddCloudLayer(cloudLayer);
		break;
	case CUMULUS_CONGESTUS:
		cloudLayer = SilverLining::CloudLayerFactory::Create(CUMULUS_CONGESTUS_HI_RES);
		cloudLayer->SetIsInfinite(true);
		cloudLayer->SetBaseAltitude(1500);
		cloudLayer->SetThickness(100);
		cloudLayer->SetBaseLength(30000);
		cloudLayer->SetBaseWidth(30000);
		cloudLayer->SetDensity(0.8);
		cloudLayer->SetLayerPosition(0, 0);
		cloudLayer->SetCloudAnimationEffects(0.2, false);
		cloudLayer->SeedClouds(*myAtmosphere);
		cloudLayer->SetAlpha(0.5);
		cloudLayer->SetFadeTowardEdges(true);

		myAtmosphere->GetConditions()->AddCloudLayer(cloudLayer);

		break;
	case CUMULUS_CONGESTUS_HI_RES:
		cloudLayer = SilverLining::CloudLayerFactory::Create(CUMULUS_CONGESTUS_HI_RES);
		cloudLayer->SetIsInfinite(true);
		cloudLayer->SetBaseAltitude(1500);
		cloudLayer->SetThickness(100);
		cloudLayer->SetBaseLength(30000);
		cloudLayer->SetBaseWidth(30000);
		cloudLayer->SetDensity(0.8);
		cloudLayer->SetLayerPosition(0, 0);
		cloudLayer->SetCloudAnimationEffects(0.2, false);
		cloudLayer->SeedClouds(*myAtmosphere);
		cloudLayer->SetAlpha(0.5);
		cloudLayer->SetFadeTowardEdges(true);

		myAtmosphere->GetConditions()->AddCloudLayer(cloudLayer);
		break;
	case CUMULONIMBUS_CAPPILATUS:
		cloudLayer = SilverLining::CloudLayerFactory::Create(CUMULONIMBUS_CAPPILATUS);
		cloudLayer->SetBaseAltitude(1000);
		cloudLayer->SetThickness(3000);
		cloudLayer->SetBaseLength(3000);
		cloudLayer->SetBaseWidth(5000);
		cloudLayer->SetLayerPosition(0, -10000);
		cloudLayer->SeedClouds(*myAtmosphere);

		myAtmosphere->GetConditions()->AddCloudLayer(cloudLayer);
		break;
	case STRATOCUMULUS:
		cloudLayer = SilverLining::CloudLayerFactory::Create(STRATOCUMULUS);
		cloudLayer->SetBaseAltitude(1000);
		cloudLayer->SetThickness(3000);
		cloudLayer->SetBaseLength(30000);
		cloudLayer->SetBaseWidth(30000);
		cloudLayer->SetDensity(0.5);
		cloudLayer->SetIsInfinite(true);
		cloudLayer->SetAlpha(1.0);
		cloudLayer->SetFadeTowardEdges(true);
		cloudLayer->SetLayerPosition(0, 0);
		cloudLayer->SeedClouds(*myAtmosphere);

		myAtmosphere->GetConditions()->AddCloudLayer(cloudLayer);
		break;
	case STRATOCUMULUS_PARTICLES:
		break;
	case TOWERING_CUMULUS:
		break;
	case SANDSTORM:
		cloudLayer = SilverLining::CloudLayerFactory::Create(SANDSTORM);
		cloudLayer->SetIsInfinite(false);
		cloudLayer->SetLayerPosition(0, -25000);
		cloudLayer->SetBaseAltitude(0);
		cloudLayer->SetBaseLength(50000);
		cloudLayer->SetBaseWidth(50000);
		cloudLayer->SeedClouds(*myAtmosphere);

		myAtmosphere->GetConditions()->AddCloudLayer(cloudLayer);
		break;

	}

	myNewCloudLayerType=-2;
}

void SilverLiningUserInterface::setPrecipitation( int precipitationType,double precipitationRate,double nearClip /*= -1*/, double farClip /*= -1*/, bool bUseDepthBuffer /*= false */ )
{
	myAtmosphere->GetConditions()->SetPrecipitation(0,0);
	myAtmosphere->GetConditions()->SetPrecipitation(precipitationType,precipitationRate,-1,farClip);
}

void SilverLiningUserInterface::setWind( double speed, double direction )
{
	myAtmosphere->GetConditions()->ClearWindVolumes();

	::SilverLining::WindVolume wv;
	wv.SetDirection(direction);
	wv.SetMinAltitude(0);
	wv.SetMaxAltitude(10000);
	wv.SetWindSpeed(speed);
	myAtmosphere->GetConditions()->SetWind(wv);
}

void SilverLiningUserInterface::onInitialize( SilverLining::Atmosphere* atmosphere )
{
	__super::onInitialize(atmosphere);

	mtVisibility=myAtmosphere->GetConditions()->GetVisibility();
}

void SilverLiningUserInterface::updateFog()
{
	if (!myFog.valid())
		return;

	float hazeDensity = 1.0 / mtVisibility;

	// Decrease fog density with altitude, to avoid fog effects through the vacuum of space.
	static const double H = 8435.0; // Pressure scale height of Earth's atmosphere
	double isothermalEffect = exp(-(myAtmosphere->GetConditions()->GetLocation().GetAltitude() / H));     
	if (isothermalEffect <= 0) isothermalEffect = 1E-9;
	if (isothermalEffect > 1.0) isothermalEffect = 1.0;
	hazeDensity *= isothermalEffect;

	bool silverLiningHandledTheFog = false;

	if (myAtmosphere->GetFogEnabled())
	{
		float density, r, g, b;
		// Note, the fog color returned is already lit
		myAtmosphere->GetFogSettings(&density, &r, &g, &b);

		if (density > hazeDensity)
		{
			myFog->setDensity(density);

			myFog->setColor(osg::Vec4(r,g,b,1.0));

			silverLiningHandledTheFog = true;
		}
	}

	if (!silverLiningHandledTheFog)
	{
		GLfloat fogColor[4];
		myAtmosphere->GetHorizonColor(0, &fogColor[0], &fogColor[1], &fogColor[2]);

		myFog->setColor(osg::Vec4(fogColor[0],fogColor[1],fogColor[2],fogColor[3]));
		myFog->setDensity(hazeDensity);

	}
}

void SilverLiningUserInterface::setVisibility( double range )
{
	mtVisibility=range;
	myAtmosphere->GetConditions()->SetVisibility(range);
}

void SilverLiningUserInterface::setFog( osg::Fog* fog )
{
	myFog=fog;
}
