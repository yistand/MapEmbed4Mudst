# MapEmbed4Mudst
Map Embedding data with Mudst for more event info

Embedding data on pdsf only have a few event information. But they have Monte Carlo tracks for TPC efficiency study.
Mudst data have more.

In order to study TPC efficiency dependence on some event variables, like global track occupancy, the event information need to be retrieved from Mudst data.
Embedding only use partial data from the all Mudst event. Therefore, the mapping between embedding and Mudst needs to be done as Mudst is very large dataset and it is time consuming to read in all data.

