#ifndef HiEgammaAlgos_EcalClusterIsoCalculator_h
#define HiEgammaAlgos_EcalClusterIsoCalculator_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"


class EcalClusterIsoCalculator
{
public:

  EcalClusterIsoCalculator(const edm::Event &iEvent, const edm::EventSetup &iSetup, const edm::Handle<reco::BasicClusterCollection> barrel, const edm::Handle<reco::BasicClusterCollection> endcap);

  /// Return the ecal cluster energy in a cone around the SC
  double getEcalClusterIso(const reco::SuperClusterRef clus, const double radius, const double threshold);
  /// Return the background-subtracted ecal cluster energy in a cone around the SC
  double getBkgSubEcalClusterIso(const reco::SuperClusterRef clus, const double radius, const double threshold);
  double getBkgSubEcalClusterIso_flowModulation(const reco::SuperClusterRef clus, const double radius, const double threshold, const edm::Handle<std::vector<double>> rhoFlowFitParams);
  double getFlowmodulation(const double phi, const double eventPlane2, const double eventPlane3, const double par1, const double par2);

private:

  const reco::BasicClusterCollection *fEBclusters_;
  const reco::BasicClusterCollection *fEEclusters_;
  const CaloGeometry                 *geometry_;
};

#endif
