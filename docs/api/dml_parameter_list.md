# TR-181 Datamodel Parameter List

> **Automation Test Suite Reference** - Generated from dml_list.txt (TR-181 v2.12, Comcast/RDK)

| Access | Count | Test Action |
|--------|-------|-------------|
| readOnly  | 356 | GET only - assert non-empty or expected value |
| readWrite | 548 | GET + SET - verify set/get round-trip |
| **Total** | **904** | |

---

| # | Parameter | Access | Data Type | Description |
|---|-----------|--------|-----------|-------------|
| 1 | `Device.DeviceInfo.AdditionalSoftwareVersion` | readOnly | string | Additional software version string reported by the device. |
| 2 | `Device.DeviceInfo.Description` | readOnly | string | Text description reported by the device. |
| 3 | `Device.DeviceInfo.FirstUseDate` | readOnly | dateTime | Date and time when the device was first put into service. |
| 4 | `Device.DeviceInfo.HardwareVersion` | readOnly | string | Version string reported for the device. |
| 5 | `Device.DeviceInfo.Manufacturer` | readOnly | string | Manufacturer reported for the device. |
| 6 | `Device.DeviceInfo.ManufacturerOUI` | readOnly | string | OUI identifying the device manufacturer. |
| 7 | `Device.DeviceInfo.MemoryStatus.Free` | readOnly | unsignedInt | Currently available memory reported by the device. |
| 8 | `Device.DeviceInfo.MemoryStatus.Total` | readOnly | unsignedInt | Total installed memory reported by the device. |
| 9 | `Device.DeviceInfo.Migration.MigrationStatus` | readOnly | string | Current migration state reported by the migration workflow. |
| 10 | `Device.DeviceInfo.MigrationPreparer.MigrationReady` | readOnly | string | Reports whether the migration preparer considers the device ready. |
| 11 | `Device.DeviceInfo.ModelName` | readOnly | string | Model identifier reported for the device. |
| 12 | `Device.DeviceInfo.ProcessStatus.CPUUsage` | readOnly | unsignedInt | Aggregate CPU usage reported for the device. |
| 13 | `Device.DeviceInfo.ProcessStatus.Process.{i}.CPUTime` | readOnly | unsignedInt | CPU time consumed by this process entry. |
| 14 | `Device.DeviceInfo.ProcessStatus.Process.{i}.Command` | readOnly | string | Command line associated with this process entry. |
| 15 | `Device.DeviceInfo.ProcessStatus.Process.{i}.PID` | readOnly | unsignedInt | Process ID for this process entry. |
| 16 | `Device.DeviceInfo.ProcessStatus.Process.{i}.Priority` | readOnly | unsignedInt | Scheduling priority for this process entry. |
| 17 | `Device.DeviceInfo.ProcessStatus.Process.{i}.Size` | readOnly | unsignedInt | Reported memory footprint for this process entry. |
| 18 | `Device.DeviceInfo.ProcessStatus.Process.{i}.State` | readOnly | string | Configuration or status value for this process entry. |
| 19 | `Device.DeviceInfo.ProcessStatus.ProcessNumberOfEntries` | readOnly | unsignedInt | Number of process entries currently exposed in the Process table. |
| 20 | `Device.DeviceInfo.Processor.{i}.Architecture` | readOnly | string | Processor architecture reported for this processor entry. |
| 21 | `Device.DeviceInfo.ProcessorNumberOfEntries` | readOnly | unsignedInt | Number of processor entries currently exposed in the Processor table. |
| 22 | `Device.DeviceInfo.ProductClass` | readOnly | string | Product class identifier reported by the device. |
| 23 | `Device.DeviceInfo.ProvisioningCode` | readWrite | string | ACS-managed provisioning code used to classify or provision the device. |
| 24 | `Device.DeviceInfo.SerialNumber` | readOnly | string | Serial number reported for the device. |
| 25 | `Device.DeviceInfo.SoftwareVersion` | readOnly | string | Version string reported for the device. |
| 26 | `Device.DeviceInfo.SupportedDataModel.1.Features` | readOnly | string | Feature set advertised for this supported data model entry. |
| 27 | `Device.DeviceInfo.SupportedDataModel.1.URL` | readOnly | string | URL used by this supported data model entry. |
| 28 | `Device.DeviceInfo.SupportedDataModel.1.URN` | readOnly | string | URN identifier for this supported data model entry. |
| 29 | `Device.DeviceInfo.SupportedDataModel.2.Features` | readWrite | string | Feature set advertised for this supported data model entry. |
| 30 | `Device.DeviceInfo.SupportedDataModel.2.URL` | readWrite | string | URL used by this supported data model entry. |
| 31 | `Device.DeviceInfo.SupportedDataModel.2.URN` | readWrite | string | URN identifier for this supported data model entry. |
| 32 | `Device.DeviceInfo.SupportedDataModelNumberOfEntries` | readOnly | unsignedInt | Number of supported data model entries reported by the device. |
| 33 | `Device.DeviceInfo.UpTime` | readOnly | unsignedInt | Seconds since the device last booted. |
| 34 | `Device.DeviceInfo.VendorLogFileNumberOfEntries` | readOnly | unsignedInt | Number of vendor log file entries exposed by the device. |
| 35 | `Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus` | readOnly | string | Current firmware download status reported by the Comcast platform. |
| 36 | `Device.DeviceInfo.X_COMCAST-COM_FirmwareFilename` | readOnly | string | Firmware filename reported by the Comcast platform. |
| 37 | `Device.DeviceInfo.X_COMCAST-COM_FirmwareToDownload` | readOnly | string | Firmware image identifier selected for download on the Comcast platform. |
| 38 | `Device.DeviceInfo.X_COMCAST-COM_PowerStatus` | readOnly | string | Current power state reported by the Comcast platform. |
| 39 | `Device.DeviceInfo.X_COMCAST-COM_Reset` | readWrite | string | Triggers the Comcast platform reset workflow. |
| 40 | `Device.DeviceInfo.X_COMCAST-COM_STB_IP` | readOnly | string | IP address currently assigned to the set-top box. |
| 41 | `Device.DeviceInfo.X_COMCAST-COM_STB_MAC` | readOnly | string | MAC address of the set-top box. |
| 42 | `Device.DeviceInfo.X_RDKCENTRAL-COM.BootStatus` | readOnly | string | Current boot status reported by the platform. |
| 43 | `Device.DeviceInfo.X_RDKCENTRAL-COM.CPUTemp` | readWrite | int | Current CPU temperature reported by the platform. |
| 44 | `Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.AppsVersion` | readWrite | string | Version string for the installed IUI applications bundle. |
| 45 | `Device.DeviceInfo.X_RDKCENTRAL-COM.IUI.Version` | readWrite | string | Version string for the installed IUI platform. |
| 46 | `Device.DeviceInfo.X_RDKCENTRAL-COM_BootTime` | readOnly | unsignedInt | Boot time reported by the platform. |
| 47 | `Device.DeviceInfo.X_RDKCENTRAL-COM_Experience` | readOnly | string | Current user experience label reported by the platform. |
| 48 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot` | readWrite | boolean | Controls whether reboot is deferred after a firmware download. |
| 49 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow` | readWrite | boolean | Triggers immediate firmware download processing. |
| 50 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadPercent` | readOnly | int | Firmware download completion percentage reported by the platform. |
| 51 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol` | readWrite | string | Protocol used for firmware download. |
| 52 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus` | readOnly | string | Current firmware download status reported by the platform. |
| 53 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL` | readWrite | string | Firmware download URL configured for the platform. |
| 54 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig` | readWrite | boolean | Controls whether Codebig is used for firmware download. |
| 55 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareFilename` | readOnly | string | Firmware filename reported by the platform. |
| 56 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload` | readWrite | string | Firmware image identifier selected for download. |
| 57 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareUpdateState` | readOnly | string | Current firmware update state reported by the platform. |
| 58 | `Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable` | readWrite | boolean | Enables or disables IP remote support. |
| 59 | `Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.IPAddr` | readOnly | string | IP address currently assigned to the IP remote interface. |
| 60 | `Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.MACAddr` | readOnly | string | MAC address of the IP remote interface. |
| 61 | `Device.DeviceInfo.X_RDKCENTRAL-COM_LastRebootReason` | readWrite | string | Reason recorded for the most recent reboot. |
| 62 | `Device.DeviceInfo.X_RDKCENTRAL-COM_Manufacturing.UIbranding` | readWrite | string | Configured UI branding label for the device. |
| 63 | `Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType` | readWrite | string | Preferred gateway type configured for the device. |
| 64 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.DownloadStatus` | readWrite | boolean | Current download status for the related feature. |
| 65 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RDKVersion` | readOnly | string | RDK software version reported by the platform. |
| 66 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.AuthService.Host` | readWrite | string | Host value used by RFC setting AuthService Host. |
| 67 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDB` | readWrite | boolean | Control flag used by RFC setting Bootstrap Control ClearDB. |
| 68 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDBEnd` | readWrite | boolean | Control flag used by RFC setting Bootstrap Control ClearDBEnd. |
| 69 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.NetflixESNprefix` | readWrite | string | RFC configuration value for Bootstrap NetflixESNprefix. |
| 70 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.OsClass` | readWrite | string | RFC configuration value for Bootstrap OsClass. |
| 71 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName` | readWrite | string | RFC configuration value for Bootstrap PartnerName. |
| 72 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName` | readWrite | string | RFC configuration value for Bootstrap PartnerProductName. |
| 73 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.SsrUrl` | readWrite | string | Endpoint URL used by RFC setting Bootstrap SsrUrl. |
| 74 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.XconfRecoveryUrl` | readWrite | string | Endpoint URL used by RFC setting Bootstrap XconfRecoveryUrl. |
| 75 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.XconfUrl` | readWrite | string | Endpoint URL used by RFC setting Bootstrap XconfUrl. |
| 76 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.ClearParam` | readWrite | string | RFC configuration value for ClearParam. |
| 77 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CloudStore.Uri` | readWrite | string | Endpoint URL used by RFC setting CloudStore Uri. |
| 78 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB` | readWrite | boolean | Control flag used by RFC setting Control ClearDB. |
| 79 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDBEnd` | readWrite | boolean | Control flag used by RFC setting Control ClearDBEnd. |
| 80 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ConfigChangeTime` | readWrite | unsignedInt | RFC configuration value for Control ConfigChangeTime. |
| 81 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ConfigSetHash` | readWrite | string | RFC configuration value for Control ConfigSetHash. |
| 82 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ConfigSetTime` | readWrite | unsignedInt | RFC configuration value for Control ConfigSetTime. |
| 83 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow` | readWrite | unsignedInt | Control flag used by RFC setting Control RetrieveNow. |
| 84 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfSelector` | readWrite | string | RFC configuration value for Control XconfSelector. |
| 85 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl` | readWrite | string | Endpoint URL used by RFC setting Control XconfUrl. |
| 86 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.S3BucketUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload S3BucketUrl. |
| 87 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.S3SigningUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload S3SigningUrl. |
| 88 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.crashPortalDEVUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload crashPortalDEVUrl. |
| 89 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.crashPortalPRODUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload crashPortalPRODUrl. |
| 90 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.crashPortalSTBUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload crashPortalSTBUrl. |
| 91 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.crashPortalVBNUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload crashPortalVBNUrl. |
| 92 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DAC.ConfigURL` | readWrite | string | Endpoint URL used by RFC setting DAC ConfigURL. |
| 93 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DAC.dacBundleFirmwareCompatibilityKey` | readWrite | string | RFC configuration value for DAC dacBundleFirmwareCompatibilityKey. |
| 94 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DAC.dacBundlePlatformName` | readWrite | string | RFC configuration value for DAC dacBundlePlatformName. |
| 95 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DynamicIUIUpdate.IUISelector` | readWrite | string | RFC configuration value for DynamicIUIUpdate IUISelector. |
| 96 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DynamicIUIUpdate.Pathway.Uri` | readWrite | string | Endpoint URL used by RFC setting DynamicIUIUpdate Pathway Uri. |
| 97 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.1080pGraphics.Enable` | readWrite | boolean | RFC flag that enables or disables Feature 1080pGraphics Enable. |
| 98 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AAMP_CFG.DashPlaybackExclusions` | readWrite | string | RFC configuration value for Feature AAMP CFG DashPlaybackExclusions. |
| 99 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AAMP_CFG.DashPlaybackInclusions` | readWrite | string | RFC configuration value for Feature AAMP CFG DashPlaybackInclusions. |
| 100 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AAMP_CFG.b64Config` | readWrite | string | RFC configuration value for Feature AAMP CFG b64Config. |
| 101 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID` | readWrite | string | RFC configuration value for Feature AccountInfo AccountID. |
| 102 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Airplay Enable. |
| 103 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AmazonPrimeVideo.SyePlayer.Enable` | readWrite | boolean | RFC flag that enables or disables Feature AmazonPrimeVideo SyePlayer Enable. |
| 104 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppConfig.App.{i}.AppName` | readWrite | string | RFC configuration value for Feature AppConfig App entry AppName. |
| 105 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppConfig.App.{i}.AppSize` | readWrite | string | RFC configuration value for Feature AppConfig App entry AppSize. |
| 106 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppConfig.App.{i}.AppUrl` | readWrite | string | Endpoint URL used by RFC setting Feature AppConfig App entry AppUrl. |
| 107 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppConfig.AppCnt` | readWrite | unsignedInt | RFC configuration value for Feature AppConfig AppCnt. |
| 108 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppConfig.Enable` | readWrite | boolean | RFC flag that enables or disables Feature AppConfig Enable. |
| 109 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppHibernate.DelayInSecLaunchedToSuspended` | readWrite | unsignedInt | RFC configuration value for Feature AppHibernate DelayInSecLaunchedToSuspended. |
| 110 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppHibernate.DelayInSecResumedToSuspended` | readWrite | unsignedInt | RFC configuration value for Feature AppHibernate DelayInSecResumedToSuspended. |
| 111 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppHibernate.Enable` | readWrite | boolean | RFC flag that enables or disables Feature AppHibernate Enable. |
| 112 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AutoReboot.Enable` | readWrite | boolean | RFC flag that enables or disables Feature AutoReboot Enable. |
| 113 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AutoReboot.fwDelayReboot` | readWrite | int | RFC configuration value for Feature AutoReboot fwDelayReboot. |
| 114 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTR.AudioIn.Enable` | readWrite | boolean | RFC flag that enables or disables Feature BTR AudioIn Enable. |
| 115 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTR.DebugMode.Enable` | readWrite | boolean | RFC flag that enables or disables Feature BTR DebugMode Enable. |
| 116 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTR.GamePad.Enable` | readWrite | boolean | RFC flag that enables or disables Feature BTR GamePad Enable. |
| 117 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTSplitAudio.Enable` | readWrite | boolean | RFC flag that enables or disables Feature BTSplitAudio Enable. |
| 118 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTSplitAudio.Language` | readWrite | string | RFC configuration value for Feature BTSplitAudio Language. |
| 119 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BootstrapConfig.Enable` | readWrite | boolean | RFC flag that enables or disables Feature BootstrapConfig Enable. |
| 120 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CDLDM.CDLModuleUrl` | readWrite | string | Endpoint URL used by RFC setting Feature CDLDM CDLModuleUrl. |
| 121 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CPC1960.Enable` | readWrite | boolean | RFC flag that enables or disables Feature CPC1960 Enable. |
| 122 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CRL.DirectOCSP` | readWrite | boolean | RFC configuration value for Feature CRL DirectOCSP. |
| 123 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CRL.Enable` | readWrite | boolean | RFC flag that enables or disables Feature CRL Enable. |
| 124 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpEnd` | readWrite | int | RFC configuration value for Feature Canary wakeUpEnd. |
| 125 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpStart` | readWrite | int | RFC configuration value for Feature Canary wakeUpStart. |
| 126 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CrashportalEndpoint.Enable` | readWrite | string | RFC flag that enables or disables Feature CrashportalEndpoint Enable. |
| 127 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CrashportalEndpoint.URL` | readWrite | string | Endpoint URL used by RFC setting Feature CrashportalEndpoint URL. |
| 128 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CredDwnld.Enable` | readWrite | string | RFC flag that enables or disables Feature CredDwnld Enable. |
| 129 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CredDwnld.Use` | readWrite | string | RFC configuration value for Feature CredDwnld Use. |
| 130 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable` | readWrite | boolean | RFC flag that enables or disables Feature DAB Enable. |
| 131 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DHCPv6Client.Enable` | readWrite | boolean | RFC flag that enables or disables Feature DHCPv6Client Enable. |
| 132 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DNSStrictOrder.Enable` | readWrite | boolean | RFC flag that enables or disables Feature DNSStrictOrder Enable. |
| 133 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DefaultSupportedLocales` | readWrite | string | RFC configuration value for Feature DefaultSupportedLocales. |
| 134 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DtmAnalytics.Enable` | readWrite | string | RFC flag that enables or disables Feature DtmAnalytics Enable. |
| 135 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DtmAnalytics.RecordsToBuffer` | readWrite | string | RFC configuration value for Feature DtmAnalytics RecordsToBuffer. |
| 136 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DtmAnalytics.ServiceUrl` | readWrite | string | Endpoint URL used by RFC setting Feature DtmAnalytics ServiceUrl. |
| 137 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.EnableHttpCDL.Enable` | readWrite | boolean | RFC flag that enables or disables Feature EnableHttpCDL Enable. |
| 138 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.EntOsAppPlatform.EnableDistPlatform` | readWrite | boolean | RFC configuration value for Feature EntOsAppPlatform EnableDistPlatform. |
| 139 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FOG_CFG.b64Config` | readWrite | string | RFC configuration value for Feature FOG CFG b64Config. |
| 140 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.Enable` | readWrite | boolean | RFC flag that enables or disables Feature FWUpdate AutoExcluded Enable. |
| 141 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.XconfUrl` | readWrite | string | Endpoint URL used by RFC setting Feature FWUpdate AutoExcluded XconfUrl. |
| 142 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HdmiCecSink.CECVersion` | readWrite | string | RFC configuration value for Feature HdmiCecSink CECVersion. |
| 143 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IDS.Enable` | readWrite | boolean | RFC flag that enables or disables Feature IDS Enable. |
| 144 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IDS.ScanTask` | readWrite | string | RFC configuration value for Feature IDS ScanTask. |
| 145 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IPControl.Service.Discovery.Enable` | readWrite | boolean | RFC flag that enables or disables Feature IPControl Service Discovery Enable. |
| 146 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IPControl.Subsystem.RICS.Enable` | readWrite | boolean | RFC flag that enables or disables Feature IPControl Subsystem RICS Enable. |
| 147 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IPRemotePort.Enable` | readWrite | boolean | RFC flag that enables or disables Feature IPRemotePort Enable. |
| 148 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.InactiveApplications.Maximum` | readWrite | unsignedInt | RFC configuration value for Feature InactiveApplications Maximum. |
| 149 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IncrementalCDL.Enable` | readWrite | boolean | RFC flag that enables or disables Feature IncrementalCDL Enable. |
| 150 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.JSPPCache.Enable` | readWrite | boolean | RFC flag that enables or disables Feature JSPPCache Enable. |
| 151 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.AdCacheEnable` | readWrite | string | RFC configuration value for Feature LSA AdCacheEnable. |
| 152 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Advt_Opt_Out_Key` | readWrite | string | RFC configuration value for Feature LSA Advt Opt Out Key. |
| 153 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.AltconReceiver` | readWrite | string | RFC configuration value for Feature LSA AltconReceiver. |
| 154 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.ByteRangeDownload` | readWrite | string | RFC configuration value for Feature LSA ByteRangeDownload. |
| 155 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Enable` | readWrite | string | RFC flag that enables or disables Feature LSA Enable. |
| 156 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.PSNUrl` | readWrite | string | Endpoint URL used by RFC setting Feature LSA PSNUrl. |
| 157 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.PlacementReqUrl` | readWrite | string | Endpoint URL used by RFC setting Feature LSA PlacementReqUrl. |
| 158 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.ProgrammerEnable` | readWrite | string | RFC configuration value for Feature LSA ProgrammerEnable. |
| 159 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Schema_Location` | readWrite | string | RFC configuration value for Feature LSA Schema Location. |
| 160 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Schemas_NGOD` | readWrite | string | RFC configuration value for Feature LSA Schemas NGOD. |
| 161 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Schemas_admin` | readWrite | string | RFC configuration value for Feature LSA Schemas admin. |
| 162 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Schemas_core` | readWrite | string | RFC configuration value for Feature LSA Schemas core. |
| 163 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.XMLNS_Schema` | readWrite | string | RFC configuration value for Feature LSA XMLNS Schema. |
| 164 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.XMLSchema_Instance` | readWrite | string | RFC configuration value for Feature LSA XMLSchema Instance. |
| 165 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Xifaid_Xml_Key` | readWrite | string | RFC configuration value for Feature LSA Xifaid Xml Key. |
| 166 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LaunchDarkly.EnvKeyLabel` | readWrite | string | RFC configuration value for Feature LaunchDarkly EnvKeyLabel. |
| 167 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LnFUseXPKI.Enable` | readWrite | boolean | RFC flag that enables or disables Feature LnFUseXPKI Enable. |
| 168 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LogUploadBeforeDeepSleep.Enable` | readWrite | boolean | RFC flag that enables or disables Feature LogUploadBeforeDeepSleep Enable. |
| 169 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LogUploadEndpoint.Enable` | readWrite | string | RFC flag that enables or disables Feature LogUploadEndpoint Enable. |
| 170 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LogUploadEndpoint.URL` | readWrite | string | Endpoint URL used by RFC setting Feature LogUploadEndpoint URL. |
| 171 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LoudnessEquivalence.Enable` | readWrite | boolean | RFC flag that enables or disables Feature LoudnessEquivalence Enable. |
| 172 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MEMSWAP.Enable` | readWrite | string | RFC flag that enables or disables Feature MEMSWAP Enable. |
| 173 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable` | readWrite | boolean | RFC flag that enables or disables Feature MOCASSH Enable. |
| 174 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DAPv2_Enable` | readWrite | boolean | RFC configuration value for Feature MS12 DAPv2 Enable. |
| 175 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DE_Enable` | readWrite | boolean | RFC configuration value for Feature MS12 DE Enable. |
| 176 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MTLS.mTlsXcSsr.Enable` | readWrite | boolean | RFC flag that enables or disables Feature MTLS mTlsXcSsr Enable. |
| 177 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ManageableNotification.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ManageableNotification Enable. |
| 178 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Miracast.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Miracast Enable. |
| 179 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NTP.failoverServer` | readWrite | string | RFC configuration value for Feature NTP failoverServer. |
| 180 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonPersistent.WebPACDL.Enable` | readWrite | boolean | RFC flag that enables or disables Feature NonPersistent WebPACDL Enable. |
| 181 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.ApparmorBlocklist` | readWrite | string | RFC configuration value for Feature NonRootSupport ApparmorBlocklist. |
| 182 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.Blocklist` | readWrite | string | RFC configuration value for Feature NonRootSupport Blocklist. |
| 183 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.Enable` | readWrite | boolean | RFC flag that enables or disables Feature NonRootSupport Enable. |
| 184 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PeriodicFWCheck.Enable` | readWrite | boolean | RFC flag that enables or disables Feature PeriodicFWCheck Enable. |
| 185 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PeriodicFwCheck.timeperiod` | readWrite | unsignedInt | RFC configuration value for Feature PeriodicFwCheck timeperiod. |
| 186 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PingTelemetry.BurstCnt` | readWrite | unsignedInt | RFC configuration value for Feature PingTelemetry BurstCnt. |
| 187 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PingTelemetry.Enable` | readWrite | boolean | RFC flag that enables or disables Feature PingTelemetry Enable. |
| 188 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PingTelemetry.EndTime` | readWrite | unsignedInt | RFC configuration value for Feature PingTelemetry EndTime. |
| 189 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PingTelemetry.StartTime` | readWrite | unsignedInt | RFC configuration value for Feature PingTelemetry StartTime. |
| 190 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PingTelemetry.Type` | readWrite | string | RFC configuration value for Feature PingTelemetry Type. |
| 191 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Power.DeepSleepNotification.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Power DeepSleepNotification Enable. |
| 192 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Power.PwrMgr2.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Power PwrMgr2 Enable. |
| 193 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Power.UserInactivityNotification.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Power UserInactivityNotification Enable. |
| 194 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Power.UserInactivityNotification.TimeMinutes` | readWrite | unsignedInt | RFC configuration value for Feature Power UserInactivityNotification TimeMinutes. |
| 195 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Preference.URL` | readWrite | string | Endpoint URL used by RFC setting Feature Preference URL. |
| 196 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PressAndRelease.EOSMethod` | readWrite | int | RFC configuration value for Feature PressAndRelease EOSMethod. |
| 197 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PressAndRelease.EOSTimeout` | readWrite | int | RFC configuration value for Feature PressAndRelease EOSTimeout. |
| 198 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ProtectX1Services.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ProtectX1Services Enable. |
| 199 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKBROWSER2_CFG.BACKGROUND_MEM_USAGE_HIGH_WATERMARK` | readWrite | unsignedInt | RFC configuration value for Feature RDKBROWSER2 CFG BACKGROUND MEM USAGE HIGH WATERMARK. |
| 200 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKBROWSER2_CFG.BACKGROUND_MEM_USAGE_LOW_WATERMARK` | readWrite | unsignedInt | RFC configuration value for Feature RDKBROWSER2 CFG BACKGROUND MEM USAGE LOW WATERMARK. |
| 201 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKBROWSER2_CFG.HTML_APP_RESURRECTION_PERIOD.days` | readWrite | unsignedInt | RFC configuration value for Feature RDKBROWSER2 CFG HTML APP RESURRECTION PERIOD days. |
| 202 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable` | readWrite | boolean | RFC flag that enables or disables Feature RDKRemoteDebugger Enable. |
| 203 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType` | readWrite | string | RFC configuration value for Feature RDKRemoteDebugger IssueType. |
| 204 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData` | readWrite | string | RFC configuration value for Feature RDKRemoteDebugger WebCfgData. |
| 205 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.getProfileData` | readOnly | string | RFC configuration value for Feature RDKRemoteDebugger getProfileData. |
| 206 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDMDecoupledVersionManagement.Enable` | readWrite | boolean | RFC flag that enables or disables Feature RDMDecoupledVersionManagement Enable. |
| 207 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.AudioProfileTarget` | readWrite | int | RFC configuration value for Feature RF4CE AudioProfileTarget. |
| 208 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.FF.RspIdle` | readWrite | int | RFC configuration value for Feature RF4CE FF RspIdle. |
| 209 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.HostPacketDecryption.Enable` | readWrite | boolean | RFC flag that enables or disables Feature RF4CE HostPacketDecryption Enable. |
| 210 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.OpusEncoderParams` | readWrite | string | RFC configuration value for Feature RF4CE OpusEncoderParams. |
| 211 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.RspTime.XDIU` | readWrite | int | RFC configuration value for Feature RF4CE RspTime XDIU. |
| 212 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.RspTime.XRC` | readWrite | int | RFC configuration value for Feature RF4CE RspTime XRC. |
| 213 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.RspTime.XVP` | readWrite | int | RFC configuration value for Feature RF4CE RspTime XVP. |
| 214 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.VoiceEncryption.Enable` | readWrite | boolean | RFC flag that enables or disables Feature RF4CE VoiceEncryption Enable. |
| 215 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Detection` | readWrite | boolean | RFC configuration value for Feature RebootStop Detection. |
| 216 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Duration` | readWrite | int | RFC configuration value for Feature RebootStop Duration. |
| 217 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Enable` | readWrite | boolean | RFC flag that enables or disables Feature RebootStop Enable. |
| 218 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RedRecovery.Status` | readWrite | string | Current value reported for RFC setting Feature RedRecovery Status. |
| 219 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Resourcemanager.Blacklist.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Resourcemanager Blacklist Enable. |
| 220 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Resourcemanager.ReserveTTS.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Resourcemanager ReserveTTS Enable. |
| 221 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RoamTrigger` | readWrite | string | RFC configuration value for Feature RoamTrigger. |
| 222 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SDCARD_SCRATCHPAD.Enable` | readWrite | string | RFC flag that enables or disables Feature SDCARD SCRATCHPAD Enable. |
| 223 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SHORTS.Enable` | readWrite | boolean | RFC flag that enables or disables Feature SHORTS Enable. |
| 224 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.STAGE.Enable` | readWrite | boolean | RFC flag that enables or disables Feature STAGE Enable. |
| 225 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLDirect.Enable` | readWrite | boolean | RFC flag that enables or disables Feature SWDLDirect Enable. |
| 226 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable` | readWrite | boolean | RFC flag that enables or disables Feature SWDLSpLimit Enable. |
| 227 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.LowSpeed` | readWrite | int | RFC configuration value for Feature SWDLSpLimit LowSpeed. |
| 228 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed` | readWrite | int | RFC configuration value for Feature SWDLSpLimit TopSpeed. |
| 229 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ScreenCapture.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ScreenCapture Enable. |
| 230 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ScreenCapture.URL` | readWrite | string | Endpoint URL used by RFC setting Feature ScreenCapture URL. |
| 231 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SecDump.Enable` | readWrite | boolean | RFC flag that enables or disables Feature SecDump Enable. |
| 232 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SoundPlayer.Enable` | readWrite | boolean | RFC flag that enables or disables Feature SoundPlayer Enable. |
| 233 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StandbyReboot.Enable` | readWrite | boolean | RFC flag that enables or disables Feature StandbyReboot Enable. |
| 234 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StandbyReboot.ForceAutoReboot` | readWrite | int | RFC configuration value for Feature StandbyReboot ForceAutoReboot. |
| 235 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StandbyReboot.StandbyAutoReboot` | readWrite | int | RFC configuration value for Feature StandbyReboot StandbyAutoReboot. |
| 236 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StorageManager.maxAdaptiveBitRate` | readWrite | int | RFC configuration value for Feature StorageManager maxAdaptiveBitRate. |
| 237 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StorageManager.maxTSBDurationMinutes` | readWrite | int | RFC configuration value for Feature StorageManager maxTSBDurationMinutes. |
| 238 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SupportedLanguages` | readWrite | string | RFC configuration value for Feature SupportedLanguages. |
| 239 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.TR069support.Enable` | readWrite | boolean | RFC flag that enables or disables Feature TR069support Enable. |
| 240 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.ConfigURL` | readWrite | string | Endpoint URL used by RFC setting Feature Telemetry ConfigURL. |
| 241 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.Version` | readWrite | string | Version value for RFC setting Feature Telemetry Version. |
| 242 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ThreadMonitorMinidump.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ThreadMonitorMinidump Enable. |
| 243 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ThreadPrioConfig.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ThreadPrioConfig Enable. |
| 244 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ThunderSecurity.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ThunderSecurity Enable. |
| 245 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.TvSettings.DynamicAutoLatency` | readWrite | boolean | RFC configuration value for Feature TvSettings DynamicAutoLatency. |
| 246 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UNII3.Enable` | readWrite | boolean | RFC flag that enables or disables Feature UNII3 Enable. |
| 247 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UPnP.Refactor.Enable` | readWrite | boolean | RFC flag that enables or disables Feature UPnP Refactor Enable. |
| 248 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UPnPxPKI.Enable` | readWrite | boolean | RFC flag that enables or disables Feature UPnPxPKI Enable. |
| 249 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.USB_AutoMount.Enable` | readWrite | string | RFC flag that enables or disables Feature USB AutoMount Enable. |
| 250 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.USB_HID.Enable` | readWrite | boolean | RFC flag that enables or disables Feature USB HID Enable. |
| 251 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UploadLogsOnUnscheduledReboot.Disable` | readWrite | boolean | RFC configuration value for Feature UploadLogsOnUnscheduledReboot Disable. |
| 252 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ViperPPUrl` | readWrite | string | Endpoint URL used by RFC setting Feature ViperPPUrl. |
| 253 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.AudioConfidenceThreshold` | readWrite | string | RFC configuration value for Feature Voice AudioConfidenceThreshold. |
| 254 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.AudioDuckingLevel` | readWrite | string | RFC configuration value for Feature Voice AudioDuckingLevel. |
| 255 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.AudioDuckingType` | readWrite | int | RFC configuration value for Feature Voice AudioDuckingType. |
| 256 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.AudioMode` | readWrite | int | RFC configuration value for Feature Voice AudioMode. |
| 257 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.AudioTiming` | readWrite | int | RFC configuration value for Feature Voice AudioTiming. |
| 258 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.KeywordSensitivity` | readWrite | string | RFC configuration value for Feature Voice KeywordSensitivity. |
| 259 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.VSDKConfiguration` | readWrite | string | RFC configuration value for Feature Voice VSDKConfiguration. |
| 260 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFi.WiFiStatsLogInterval` | readWrite | unsignedInt | RFC configuration value for Feature WiFi WiFiStatsLogInterval. |
| 261 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.Enable` | readWrite | boolean | RFC flag that enables or disables Feature WiFiReset Enable. |
| 262 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.EthernetLoggingInterval` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset EthernetLoggingInterval. |
| 263 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.PacketLossLoggingInterval` | readWrite | string | RFC configuration value for Feature WiFiReset PacketLossLoggingInterval. |
| 264 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.ReassociateTolerance` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset ReassociateTolerance. |
| 265 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.WifiLoggingInterval` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset WifiLoggingInterval. |
| 266 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.WifiReassociateInterval` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset WifiReassociateInterval. |
| 267 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.WifiResetIntervalForDriverIssue` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset WifiResetIntervalForDriverIssue. |
| 268 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.WifiResetIntervalForPacketLoss` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset WifiResetIntervalForPacketLoss. |
| 269 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WifiOptimizer.Enable` | readWrite | boolean | RFC flag that enables or disables Feature WifiOptimizer Enable. |
| 270 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XDial.AppList` | readWrite | string | RFC configuration value for Feature XDial AppList. |
| 271 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRDsp.XR19.Configuration` | readWrite | string | RFC configuration value for Feature XRDsp XR19 Configuration. |
| 272 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPairing.ASBDerivationMethod` | readWrite | int | RFC configuration value for Feature XRPairing ASBDerivationMethod. |
| 273 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPairing.ASBEnable` | readWrite | boolean | RFC configuration value for Feature XRPairing ASBEnable. |
| 274 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPairing.ASBFailThreshold` | readWrite | int | RFC configuration value for Feature XRPairing ASBFailThreshold. |
| 275 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.Default` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration Default. |
| 276 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.Enable` | readWrite | boolean | RFC flag that enables or disables Feature XRPollingConfiguration Enable. |
| 277 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XR11v2` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XR11v2. |
| 278 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XR15v1` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XR15v1. |
| 279 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XR15v2` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XR15v2. |
| 280 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XR16v1` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XR16v1. |
| 281 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XR19v1` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XR19v1. |
| 282 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XRAv1` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XRAv1. |
| 283 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRmacPolling.Enable` | readWrite | boolean | RFC flag that enables or disables Feature XRmacPolling Enable. |
| 284 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRmacPolling.macPollingInterval` | readWrite | int | RFC configuration value for Feature XRmacPolling macPollingInterval. |
| 285 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.collectd.Enable` | readWrite | boolean | RFC flag that enables or disables Feature collectd Enable. |
| 286 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.collectd.GraphiteURL` | readWrite | string | Endpoint URL used by RFC setting Feature collectd GraphiteURL. |
| 287 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.collectd.Hostname` | readWrite | string | RFC configuration value for Feature collectd Hostname. |
| 288 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.collectd.PortNumber` | readWrite | unsignedInt | RFC configuration value for Feature collectd PortNumber. |
| 289 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.device_update` | readWrite | string | RFC configuration value for Feature ctrlm device update. |
| 290 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.global` | readWrite | string | RFC configuration value for Feature ctrlm global. |
| 291 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.network_ble` | readWrite | string | RFC configuration value for Feature ctrlm network ble. |
| 292 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.network_ip` | readWrite | string | RFC configuration value for Feature ctrlm network ip. |
| 293 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.network_rf4ce` | readWrite | string | RFC configuration value for Feature ctrlm network rf4ce. |
| 294 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.telemetry_report.ble` | readWrite | string | RFC configuration value for Feature ctrlm telemetry report ble. |
| 295 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.telemetry_report.global` | readWrite | string | RFC configuration value for Feature ctrlm telemetry report global. |
| 296 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.telemetry_report.ip` | readWrite | string | RFC configuration value for Feature ctrlm telemetry report ip. |
| 297 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.telemetry_report.rf4ce` | readWrite | string | RFC configuration value for Feature ctrlm telemetry report rf4ce. |
| 298 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.telemetry_report.voice` | readWrite | string | RFC configuration value for Feature ctrlm telemetry report voice. |
| 299 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.voice` | readWrite | string | RFC configuration value for Feature ctrlm voice. |
| 300 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.vsdk` | readWrite | string | RFC configuration value for Feature ctrlm vsdk. |
| 301 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.eMMCFirmware.Version` | readWrite | string | Version value for RFC setting Feature eMMCFirmware Version. |
| 302 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.eMMCMitigation.Disable` | readWrite | boolean | RFC configuration value for Feature eMMCMitigation Disable. |
| 303 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.fkpskdf.Enable` | readWrite | boolean | RFC flag that enables or disables Feature fkpskdf Enable. |
| 304 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.memcapture.Duration` | readWrite | int | RFC configuration value for Feature memcapture Duration. |
| 305 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.memcapture.Enable` | readWrite | boolean | RFC flag that enables or disables Feature memcapture Enable. |
| 306 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.systemd-analyze.Enable` | readWrite | boolean | RFC flag that enables or disables Feature systemd analyze Enable. |
| 307 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.xMemInsight.Args` | readWrite | string | RFC configuration value for Feature xMemInsight Args. |
| 308 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.xMemInsight.Enable` | readWrite | boolean | RFC flag that enables or disables Feature xMemInsight Enable. |
| 309 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.HostIf.ParodusTokenServerUrl` | readWrite | string | Endpoint URL used by RFC setting HostIf ParodusTokenServerUrl. |
| 310 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Identity.DbgServices.Enable` | readWrite | boolean | RFC flag that enables or disables Identity DbgServices Enable. |
| 311 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Identity.DeviceType` | readWrite | string | RFC configuration value for Identity DeviceType. |
| 312 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcaUploadPRODUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcaUploadPRODUrl. |
| 313 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcaUploadUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcaUploadUrl. |
| 314 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcmLogServerCQAUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcmLogServerCQAUrl. |
| 315 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcmLogServerDEVUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcmLogServerDEVUrl. |
| 316 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcmLogServerPRODUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcmLogServerPRODUrl. |
| 317 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcmLogUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcmLogUrl. |
| 318 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcmScpServerUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcmScpServerUrl. |
| 319 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.LogServerUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload LogServerUrl. |
| 320 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.S3SignedBucketUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload S3SignedBucketUrl. |
| 321 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.SsrUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload SsrUrl. |
| 322 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SecManager.dieselUrl` | readWrite | string | Endpoint URL used by RFC setting SecManager dieselUrl. |
| 323 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.Activation` | readWrite | string | RFC configuration value for SocProvisioning Activation. |
| 324 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.AuthMessage` | readWrite | string | RFC configuration value for SocProvisioning AuthMessage. |
| 325 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.NameSpacePrefix` | readWrite | string | RFC configuration value for SocProvisioning NameSpacePrefix. |
| 326 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.NameSpaceUri` | readWrite | string | RFC configuration value for SocProvisioning NameSpaceUri. |
| 327 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.Renewal` | readWrite | string | RFC configuration value for SocProvisioning Renewal. |
| 328 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.URL1` | readWrite | string | RFC configuration value for SocProvisioning URL1. |
| 329 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.URL2` | readWrite | string | RFC configuration value for SocProvisioning URL2. |
| 330 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.backoffIntervalMax` | readWrite | unsignedLong | RFC configuration value for SocProvisioning backoffIntervalMax. |
| 331 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.disableCredentialsPrefetchCaching` | readWrite | boolean | RFC configuration value for SocProvisioning disableCredentialsPrefetchCaching. |
| 332 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.CCRProxyUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint CCRProxyUrl. |
| 333 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.CDNCCPUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint CDNCCPUrl. |
| 334 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.CcpUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint CcpUrl. |
| 335 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.DAC15CDLUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint DAC15CDLUrl. |
| 336 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.EDGEDNSProxyUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint EDGEDNSProxyUrl. |
| 337 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.FkpsBrokerUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint FkpsBrokerUrl. |
| 338 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.FkpsUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint FkpsUrl. |
| 339 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.PRODCDLUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint PRODCDLUrl. |
| 340 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.XcalUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint XcalUrl. |
| 341 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.XconfDEVUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint XconfDEVUrl. |
| 342 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.XconfUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint XconfUrl. |
| 343 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.FriendlyName` | readWrite | string | RFC configuration value for SystemServices FriendlyName. |
| 344 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TextToSpeech.URL` | readWrite | string | Endpoint URL used by RFC setting TextToSpeech URL. |
| 345 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.LRHAcceptValue` | readWrite | string | RFC configuration value for aamp LRHAcceptValue. |
| 346 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.LRHContentType` | readWrite | string | RFC configuration value for aamp LRHContentType. |
| 347 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.SchemeIdUriDaiStream` | readWrite | string | RFC configuration value for aamp SchemeIdUriDaiStream. |
| 348 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.SchemeIdUriVssStream` | readWrite | string | RFC configuration value for aamp SchemeIdUriVssStream. |
| 349 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.SlowMotion` | readWrite | boolean | RFC configuration value for aamp SlowMotion. |
| 350 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.asyncTune` | readWrite | boolean | RFC configuration value for aamp asyncTune. |
| 351 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.cdvrLiveOffset` | readWrite | unsignedLong | RFC configuration value for aamp cdvrLiveOffset. |
| 352 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.client-dai` | readWrite | boolean | RFC configuration value for aamp client dai. |
| 353 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.curlStore` | readWrite | boolean | RFC configuration value for aamp curlStore. |
| 354 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.defaultBitrate` | readWrite | unsignedLong | RFC configuration value for aamp defaultBitrate. |
| 355 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.defaultBitrate4K` | readWrite | unsignedLong | RFC configuration value for aamp defaultBitrate4K. |
| 356 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.disableAC3` | readWrite | boolean | RFC configuration value for aamp disableAC3. |
| 357 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.disableAC4` | readWrite | boolean | RFC configuration value for aamp disableAC4. |
| 358 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.disableATMOS` | readWrite | boolean | RFC configuration value for aamp disableATMOS. |
| 359 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.disableEC3` | readWrite | boolean | RFC configuration value for aamp disableEC3. |
| 360 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enableCMCD` | readWrite | boolean | RFC configuration value for aamp enableCMCD. |
| 361 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enableLiveLatencyCorrection` | readWrite | boolean | RFC configuration value for aamp enableLiveLatencyCorrection. |
| 362 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enableLowLatencyCorrection` | readWrite | boolean | RFC configuration value for aamp enableLowLatencyCorrection. |
| 363 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enableLowLatencyDash` | readWrite | boolean | RFC configuration value for aamp enableLowLatencyDash. |
| 364 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enablePTSReStamp` | readWrite | boolean | RFC configuration value for aamp enablePTSReStamp. |
| 365 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enableVideoEndEvent` | readWrite | boolean | RFC configuration value for aamp enableVideoEndEvent. |
| 366 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.info` | readWrite | boolean | RFC configuration value for aamp info. |
| 367 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.liveOffset` | readWrite | unsignedLong | RFC configuration value for aamp liveOffset. |
| 368 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.liveOffset4K` | readWrite | string | RFC configuration value for aamp liveOffset4K. |
| 369 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.manifestTimeout` | readWrite | unsignedLong | RFC configuration value for aamp manifestTimeout. |
| 370 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.maxBitrate` | readWrite | unsignedLong | RFC configuration value for aamp maxBitrate. |
| 371 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.maxInitFragCachePerTrack` | readWrite | int | RFC configuration value for aamp maxInitFragCachePerTrack. |
| 372 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.minBitrate` | readWrite | unsignedLong | RFC configuration value for aamp minBitrate. |
| 373 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.networkTimeout` | readWrite | unsignedLong | RFC configuration value for aamp networkTimeout. |
| 374 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.persistBitrateOverSeek` | readWrite | boolean | RFC configuration value for aamp persistBitrateOverSeek. |
| 375 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.playlistTimeout` | readWrite | unsignedLong | RFC configuration value for aamp playlistTimeout. |
| 376 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.preferredDrm` | readWrite | int | RFC configuration value for aamp preferredDrm. |
| 377 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.rateCorrectionDelay` | readWrite | unsignedLong | RFC configuration value for aamp rateCorrectionDelay. |
| 378 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.sharedSSL` | readWrite | boolean | RFC configuration value for aamp sharedSSL. |
| 379 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.stereoOnly` | readWrite | boolean | RFC configuration value for aamp stereoOnly. |
| 380 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.supportTLS` | readWrite | unsignedLong | RFC configuration value for aamp supportTLS. |
| 381 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.tsbInterruptHandling` | readWrite | boolean | RFC configuration value for aamp tsbInterruptHandling. |
| 382 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.useSecManager` | readWrite | boolean | RFC configuration value for aamp useSecManager. |
| 383 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.useWesterosSink` | readWrite | boolean | RFC configuration value for aamp useWesterosSink. |
| 384 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.apps.watchdogmode` | readWrite | boolean | RFC configuration value for apps watchdogmode. |
| 385 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.fog.ZeroDrmHost` | readWrite | string | RFC configuration value for fog ZeroDrmHost. |
| 386 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.recorder.LongpollUrl` | readWrite | string | Endpoint URL used by RFC setting recorder LongpollUrl. |
| 387 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.recorder.Status` | readWrite | string | Current value reported for RFC setting recorder Status. |
| 388 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.recorder.UpdateSchedule` | readWrite | string | RFC configuration value for recorder UpdateSchedule. |
| 389 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.vodclient.StunnelConnect` | readWrite | string | RFC configuration value for vodclient StunnelConnect. |
| 390 | `Device.DeviceInfo.X_RDKCENTRAL-COM_Reset` | readWrite | string | Triggers the platform-specific reset workflow. |
| 391 | `Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId` | readWrite | string | Partner identifier used for syndication. |
| 392 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.LastResetTime` | readOnly | unsignedLong | Timestamp of the most recent reset event. |
| 393 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.Status` | readOnly | string | Current status of the device. |
| 394 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.Active` | readOnly | int | Indicates whether the related entry is currently active. |
| 395 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.DeviceID` | readOnly | string | Device identifier reported for the related entry. |
| 396 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.DeviceType` | readOnly | string | Device type reported for the related entry. |
| 397 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.Name` | readOnly | string | Name reported for this connected Bluetooth device. |
| 398 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDeviceCnt` | readOnly | unsignedInt | Number of currently connected Bluetooth device entries. |
| 399 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.DeviceID` | readOnly | string | Device identifier reported for the related entry. |
| 400 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.MAC` | readOnly | string | MAC address associated with the local Bluetooth adapter. |
| 401 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.Manufacturer` | readOnly | int | Manufacturer reported for the local Bluetooth adapter. |
| 402 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.Profile` | readOnly | string | Profile reported for the related entry. |
| 403 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.RSSI` | readOnly | int | RSSI reported for the related entry. |
| 404 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.SignalStrength` | readOnly | string | Signal strength reported for the related entry. |
| 405 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.DeviceID` | readOnly | string | Device identifier reported for the related entry. |
| 406 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.DeviceType` | readOnly | string | Device type reported for the related entry. |
| 407 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.Name` | readOnly | string | Name reported for this discovered Bluetooth device. |
| 408 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.Paired` | readOnly | boolean | Indicates whether the related Bluetooth device is paired. |
| 409 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDeviceCnt` | readOnly | unsignedInt | Number of discovered Bluetooth device entries currently reported. |
| 410 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveryEnabled` | readOnly | boolean | Reports whether Bluetooth discovery is currently enabled. |
| 411 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.Enabled` | readOnly | string | Current enabled state of the device. |
| 412 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.GetDeviceInfo` | readOnly | string | Snapshot of detailed Bluetooth adapter information. |
| 413 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.LimitBeaconDetection` | readWrite | boolean | Controls whether Bluetooth beacon detection is restricted by platform policy. |
| 414 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.Connected` | readOnly | boolean | Indicates whether the related device is currently connected. |
| 415 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.DeviceID` | readOnly | string | Device identifier reported for the related entry. |
| 416 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.DeviceType` | readOnly | string | Device type reported for the related entry. |
| 417 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.Name` | readOnly | string | Name reported for this paired Bluetooth device. |
| 418 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDeviceCnt` | readOnly | unsignedInt | Number of paired Bluetooth device entries currently reported. |
| 419 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable` | readWrite | boolean | Enables or disables the xOps forward SSH access path. |
| 420 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMLogsUploadStatus` | readOnly | string | Current xOps device management log upload status. |
| 421 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification` | readWrite | string | Manageable-device notification payload for xOps RPC. |
| 422 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification` | readWrite | boolean | xOps RPC notification control for the named event. |
| 423 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification` | readWrite | string | xOps RPC notification control for the named event. |
| 424 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification` | readWrite | unsignedInt | xOps RPC notification control for the named event. |
| 425 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs` | readWrite | string | Arguments used to start reverse SSH through xOps. |
| 426 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshStatus` | readOnly | string | Current reverse SSH status reported by xOps. |
| 427 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.Results` | readOnly | string | Result payload reported by the related diagnostic or test. |
| 428 | `Device.DeviceInfo.X_RDK_FirmwareName` | readOnly | string | Firmware image name currently reported by the RDK platform. |
| 429 | `Device.DeviceInfo.X_RDK_RDKProfileName` | readWrite | string | Active RDK profile name associated with the device configuration. |
| 430 | `Device.Ethernet.Interface.{i}.Alias` | readWrite | string | User-assigned alias for this Ethernet interface. |
| 431 | `Device.Ethernet.Interface.{i}.DuplexMode` | readWrite | string | Configured or reported duplex mode for the interface. |
| 432 | `Device.Ethernet.Interface.{i}.Enable` | readWrite | boolean | Enables or disables this Ethernet interface. |
| 433 | `Device.Ethernet.Interface.{i}.LastChange` | readOnly | unsignedInt | Seconds since this Ethernet interface last changed state. |
| 434 | `Device.Ethernet.Interface.{i}.LowerLayers` | readWrite | string | Lower-layer interface references for this Ethernet interface. |
| 435 | `Device.Ethernet.Interface.{i}.MACAddress` | readOnly | string | MAC address associated with this Ethernet interface. |
| 436 | `Device.Ethernet.Interface.{i}.MaxBitRate` | readWrite | int | Configured or negotiated maximum link bit rate for this Ethernet interface. |
| 437 | `Device.Ethernet.Interface.{i}.Name` | readOnly | string | Name reported for this Ethernet interface. |
| 438 | `Device.Ethernet.Interface.{i}.Stats.BroadcastPacketsReceived` | readOnly | unsignedLong | Broadcast packets received on this Ethernet interface. |
| 439 | `Device.Ethernet.Interface.{i}.Stats.BroadcastPacketsSent` | readOnly | unsignedLong | Broadcast packets sent on this Ethernet interface. |
| 440 | `Device.Ethernet.Interface.{i}.Stats.BytesReceived` | readOnly | unsignedLong | Total bytes received on this Ethernet interface. |
| 441 | `Device.Ethernet.Interface.{i}.Stats.BytesSent` | readOnly | unsignedLong | Total bytes sent on this Ethernet interface. |
| 442 | `Device.Ethernet.Interface.{i}.Stats.DiscardPacketsReceived` | readOnly | unsignedInt | Received packets discarded on this Ethernet interface. |
| 443 | `Device.Ethernet.Interface.{i}.Stats.DiscardPacketsSent` | readOnly | unsignedInt | Outbound packets discarded on this Ethernet interface. |
| 444 | `Device.Ethernet.Interface.{i}.Stats.ErrorsReceived` | readOnly | unsignedInt | Receive errors seen on this Ethernet interface. |
| 445 | `Device.Ethernet.Interface.{i}.Stats.ErrorsSent` | readOnly | unsignedInt | Transmit errors seen on this Ethernet interface. |
| 446 | `Device.Ethernet.Interface.{i}.Stats.MulticastPacketsReceived` | readOnly | unsignedLong | Multicast packets received on this Ethernet interface. |
| 447 | `Device.Ethernet.Interface.{i}.Stats.MulticastPacketsSent` | readOnly | unsignedLong | Multicast packets sent on this Ethernet interface. |
| 448 | `Device.Ethernet.Interface.{i}.Stats.PacketsReceived` | readOnly | unsignedLong | Total packets received on this Ethernet interface. |
| 449 | `Device.Ethernet.Interface.{i}.Stats.PacketsSent` | readOnly | unsignedLong | Total packets sent on this Ethernet interface. |
| 450 | `Device.Ethernet.Interface.{i}.Stats.UnicastPacketsReceived` | readOnly | unsignedLong | Unicast packets received on this Ethernet interface. |
| 451 | `Device.Ethernet.Interface.{i}.Stats.UnicastPacketsSent` | readOnly | unsignedLong | Unicast packets sent on this Ethernet interface. |
| 452 | `Device.Ethernet.Interface.{i}.Stats.UnknownProtoPacketsReceived` | readOnly | unsignedInt | Packets with unknown protocol received on this Ethernet interface. |
| 453 | `Device.Ethernet.Interface.{i}.Status` | readOnly | string | Current status of this Ethernet interface. |
| 454 | `Device.Ethernet.Interface.{i}.Upstream` | readOnly | boolean | Indicates whether the interface is designated as upstream. |
| 455 | `Device.Ethernet.InterfaceNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 456 | `Device.Ethernet.Link.{i}.Enable` | readWrite | boolean | Enables or disables this Ethernet link. |
| 457 | `Device.Ethernet.Link.{i}.LowerLayers` | readWrite | string | Lower-layer interface references for this Ethernet link. |
| 458 | `Device.Ethernet.Link.{i}.MACAddress` | readOnly | string | MAC address associated with this Ethernet link. |
| 459 | `Device.Ethernet.Link.{i}.Name` | readOnly | string | Name reported for this Ethernet link. |
| 460 | `Device.Ethernet.Link.{i}.Status` | readOnly | string | Current status of this Ethernet link. |
| 461 | `Device.Ethernet.LinkNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 462 | `Device.IP.ActivePort.{i}.LocalIPAddress` | readOnly | string | IP address used by this active port entry. |
| 463 | `Device.IP.ActivePort.{i}.LocalPort` | readOnly | unsignedInt | Port value used by this active port entry. |
| 464 | `Device.IP.ActivePort.{i}.RemoteIPAddress` | readOnly | string | IP address used by this active port entry. |
| 465 | `Device.IP.ActivePort.{i}.RemotePort` | readOnly | unsignedInt | Port value used by this active port entry. |
| 466 | `Device.IP.ActivePort.{i}.Status` | readOnly | string | Current status of this active port entry. |
| 467 | `Device.IP.ActivePortNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 468 | `Device.IP.Diagnostics.DownloadDiagnostics.BOMTime` | readOnly | dateTime | Beginning of measurement time for the diagnostic run. |
| 469 | `Device.IP.Diagnostics.DownloadDiagnostics.DSCP` | readWrite | unsignedInt | DSCP value used by the diagnostic traffic. |
| 470 | `Device.IP.Diagnostics.DownloadDiagnostics.DiagnosticsState` | readWrite | string | Execution state of the diagnostic. |
| 471 | `Device.IP.Diagnostics.DownloadDiagnostics.DownloadTransports` | readOnly | string | Transfer transports supported by the diagnostic. |
| 472 | `Device.IP.Diagnostics.DownloadDiagnostics.DownloadURL` | readWrite | string | Target URL used by the diagnostic. |
| 473 | `Device.IP.Diagnostics.DownloadDiagnostics.EOMTime` | readOnly | dateTime | End of measurement time for the diagnostic run. |
| 474 | `Device.IP.Diagnostics.DownloadDiagnostics.EthernetPriority` | readWrite | unsignedInt | Ethernet priority used by the diagnostic traffic. |
| 475 | `Device.IP.Diagnostics.DownloadDiagnostics.Interface` | readWrite | string | Interface reference used by the download diagnostic. |
| 476 | `Device.IP.Diagnostics.DownloadDiagnostics.ROMTime` | readOnly | dateTime | Request start time for the diagnostic run. |
| 477 | `Device.IP.Diagnostics.DownloadDiagnostics.TCPOpenRequestTime` | readOnly | dateTime | Timestamp when the diagnostic opened the TCP connection request. |
| 478 | `Device.IP.Diagnostics.DownloadDiagnostics.TCPOpenResponseTime` | readOnly | dateTime | Timestamp when the diagnostic received the TCP connection response. |
| 479 | `Device.IP.Diagnostics.DownloadDiagnostics.TestBytesReceived` | readOnly | unsignedInt | Configured or measured test payload size for the diagnostic. |
| 480 | `Device.IP.Diagnostics.DownloadDiagnostics.TotalBytesReceived` | readOnly | unsignedInt | Total payload bytes transferred during the diagnostic. |
| 481 | `Device.IP.Diagnostics.IPPing.AverageResponseTime` | readOnly | unsignedInt | Measured response time reported by the diagnostic. |
| 482 | `Device.IP.Diagnostics.IPPing.DSCP` | readWrite | unsignedInt | DSCP value used by the diagnostic traffic. |
| 483 | `Device.IP.Diagnostics.IPPing.DataBlockSize` | readWrite | unsignedInt | Payload size used by the diagnostic packets. |
| 484 | `Device.IP.Diagnostics.IPPing.DiagnosticsState` | readWrite | string | Execution state of the diagnostic. |
| 485 | `Device.IP.Diagnostics.IPPing.FailureCount` | readOnly | unsignedInt | Count of failed or successful attempts in the diagnostic run. |
| 486 | `Device.IP.Diagnostics.IPPing.Host` | readWrite | string | Host name or address used by the IP ping diagnostic. |
| 487 | `Device.IP.Diagnostics.IPPing.Interface` | readWrite | string | Interface reference used by the IP ping diagnostic. |
| 488 | `Device.IP.Diagnostics.IPPing.MaximumResponseTime` | readOnly | unsignedInt | Measured response time reported by the diagnostic. |
| 489 | `Device.IP.Diagnostics.IPPing.MinimumResponseTime` | readOnly | unsignedInt | Measured response time reported by the diagnostic. |
| 490 | `Device.IP.Diagnostics.IPPing.NumberOfRepetitions` | readWrite | unsignedInt | Number of attempts configured for the diagnostic. |
| 491 | `Device.IP.Diagnostics.IPPing.SuccessCount` | readOnly | unsignedInt | Count of failed or successful attempts in the diagnostic run. |
| 492 | `Device.IP.Diagnostics.IPPing.Timeout` | readWrite | unsignedInt | Timeout value used by the diagnostic run. |
| 493 | `Device.IP.Diagnostics.TraceRoute.DSCP` | readWrite | unsignedInt | DSCP value used by the diagnostic traffic. |
| 494 | `Device.IP.Diagnostics.TraceRoute.DataBlockSize` | readWrite | unsignedInt | Payload size used by the diagnostic packets. |
| 495 | `Device.IP.Diagnostics.TraceRoute.DiagnosticsState` | readWrite | string | Execution state of the diagnostic. |
| 496 | `Device.IP.Diagnostics.TraceRoute.Host` | readWrite | string | Host name or address used by the traceroute diagnostic. |
| 497 | `Device.IP.Diagnostics.TraceRoute.Interface` | readWrite | string | Interface reference used by the traceroute diagnostic. |
| 498 | `Device.IP.Diagnostics.TraceRoute.MaxHopCount` | readWrite | unsignedInt | Maximum hop count allowed for the traceroute run. |
| 499 | `Device.IP.Diagnostics.TraceRoute.NumberOfTries` | readWrite | unsignedInt | Number of attempts configured for the diagnostic. |
| 500 | `Device.IP.Diagnostics.TraceRoute.ResponseTime` | readOnly | unsignedInt | Measured response time reported by the diagnostic. |
| 501 | `Device.IP.Diagnostics.TraceRoute.RouteHops.{i}.ErrorCode` | readOnly | unsignedInt | Error code reported for this traceroute hop. |
| 502 | `Device.IP.Diagnostics.TraceRoute.RouteHops.{i}.Host` | readOnly | string | Host name or address used by this traceroute hop. |
| 503 | `Device.IP.Diagnostics.TraceRoute.RouteHops.{i}.HostAddress` | readOnly | string | Resolved host address for this traceroute hop. |
| 504 | `Device.IP.Diagnostics.TraceRoute.RouteHops.{i}.RTTimes` | readOnly | string | Round-trip time samples for this traceroute hop. |
| 505 | `Device.IP.Diagnostics.TraceRoute.RouteHopsNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the traceroute diagnostic. |
| 506 | `Device.IP.Diagnostics.TraceRoute.Timeout` | readWrite | unsignedInt | Timeout value used by the diagnostic run. |
| 507 | `Device.IP.Diagnostics.UDPEchoConfig.BytesReceived` | readOnly | unsignedInt | Total bytes received by the related diagnostic or service. |
| 508 | `Device.IP.Diagnostics.UDPEchoConfig.BytesResponded` | readOnly | unsignedInt | Total bytes sent in response by the related diagnostic or service. |
| 509 | `Device.IP.Diagnostics.UDPEchoConfig.EchoPlusEnabled` | readWrite | boolean | Echo Plus capability state for the UDP echo server. |
| 510 | `Device.IP.Diagnostics.UDPEchoConfig.EchoPlusSupported` | readOnly | boolean | Echo Plus capability state for the UDP echo server. |
| 511 | `Device.IP.Diagnostics.UDPEchoConfig.Enable` | readWrite | boolean | Enables or disables the UDP echo server. |
| 512 | `Device.IP.Diagnostics.UDPEchoConfig.Interface` | readWrite | string | Interface reference used by the UDP echo server. |
| 513 | `Device.IP.Diagnostics.UDPEchoConfig.PacketsReceived` | readOnly | unsignedInt | Packet count recorded by the UDP echo server. |
| 514 | `Device.IP.Diagnostics.UDPEchoConfig.PacketsResponded` | readOnly | unsignedInt | Packet count recorded by the UDP echo server. |
| 515 | `Device.IP.Diagnostics.UDPEchoConfig.SourceIPAddress` | readWrite | string | IP address used by the UDP echo server. |
| 516 | `Device.IP.Diagnostics.UDPEchoConfig.TimeFirstPacketReceived` | readOnly | dateTime | Timestamp of the first or last packet seen by the UDP echo server. |
| 517 | `Device.IP.Diagnostics.UDPEchoConfig.TimeLastPacketReceived` | readOnly | dateTime | Timestamp of the first or last packet seen by the UDP echo server. |
| 518 | `Device.IP.Diagnostics.UDPEchoConfig.UDPPort` | readWrite | unsignedInt | Port value used by the UDP echo server. |
| 519 | `Device.IP.Diagnostics.UploadDiagnostics.BOMTime` | readOnly | dateTime | Beginning of measurement time for the diagnostic run. |
| 520 | `Device.IP.Diagnostics.UploadDiagnostics.DSCP` | readWrite | unsignedInt | DSCP value used by the diagnostic traffic. |
| 521 | `Device.IP.Diagnostics.UploadDiagnostics.DiagnosticsState` | readWrite | string | Execution state of the diagnostic. |
| 522 | `Device.IP.Diagnostics.UploadDiagnostics.EOMTime` | readOnly | dateTime | End of measurement time for the diagnostic run. |
| 523 | `Device.IP.Diagnostics.UploadDiagnostics.EthernetPriority` | readWrite | unsignedInt | Ethernet priority used by the diagnostic traffic. |
| 524 | `Device.IP.Diagnostics.UploadDiagnostics.Interface` | readWrite | string | Interface reference used by the upload diagnostic. |
| 525 | `Device.IP.Diagnostics.UploadDiagnostics.ROMTime` | readOnly | dateTime | Request start time for the diagnostic run. |
| 526 | `Device.IP.Diagnostics.UploadDiagnostics.TCPOpenRequestTime` | readOnly | dateTime | Timestamp when the diagnostic opened the TCP connection request. |
| 527 | `Device.IP.Diagnostics.UploadDiagnostics.TCPOpenResponseTime` | readOnly | dateTime | Timestamp when the diagnostic received the TCP connection response. |
| 528 | `Device.IP.Diagnostics.UploadDiagnostics.TestFileLength` | readWrite | unsignedInt | Configured or measured test payload size for the diagnostic. |
| 529 | `Device.IP.Diagnostics.UploadDiagnostics.TotalBytesSent` | readOnly | unsignedInt | Total payload bytes transferred during the diagnostic. |
| 530 | `Device.IP.Diagnostics.UploadDiagnostics.UploadTransports` | readOnly | string | Transfer transports supported by the diagnostic. |
| 531 | `Device.IP.Diagnostics.UploadDiagnostics.UploadURL` | readWrite | string | Target URL used by the diagnostic. |
| 532 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Argument` | readWrite | string | Input value used by the RDK speed test. |
| 533 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Authentication` | readWrite | string | Input value used by the RDK speed test. |
| 534 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.ClientType` | readWrite | unsignedInt | Client type used by the RDK speed test. |
| 535 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Enable` | readWrite | boolean | Enables or disables the RDK speed test. |
| 536 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Enable_Speedtest` | readWrite | boolean | Configuration or status value for the RDK speed test. |
| 537 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Run` | readWrite | boolean | Triggers immediate execution of the related diagnostic or action. |
| 538 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Status` | readOnly | unsignedInt | Current status of the RDK speed test. |
| 539 | `Device.IP.IPv4Capable` | readOnly | boolean | Indicates whether the device supports IPv4. |
| 540 | `Device.IP.IPv4Enable` | readWrite | boolean | Enables or disables IPv4 on this object. |
| 541 | `Device.IP.IPv4Status` | readOnly | string | Current IPv4 operational status of the device. |
| 542 | `Device.IP.Interface.{i}.Alias` | readWrite | string | User-assigned alias for this IP interface. |
| 543 | `Device.IP.Interface.{i}.AutoIPEnable` | readWrite | boolean | Enables or disables AutoIP on this IP interface. |
| 544 | `Device.IP.Interface.{i}.Enable` | readWrite | boolean | Enables or disables this IP interface. |
| 545 | `Device.IP.Interface.{i}.IPv4Address.{i}.AddressingType` | readOnly | string | Addressing method used for this IPv4 address entry. |
| 546 | `Device.IP.Interface.{i}.IPv4Address.{i}.Alias` | readWrite | string | User-assigned alias for this IPv4 address entry. |
| 547 | `Device.IP.Interface.{i}.IPv4Address.{i}.Enable` | readWrite | boolean | Enables or disables this IPv4 address entry. |
| 548 | `Device.IP.Interface.{i}.IPv4Address.{i}.IPAddress` | readWrite | string | IP address associated with this IPv4 address entry. |
| 549 | `Device.IP.Interface.{i}.IPv4Address.{i}.Status` | readOnly | string | Current status of this IPv4 address entry. |
| 550 | `Device.IP.Interface.{i}.IPv4Address.{i}.SubnetMask` | readWrite | string | Subnet mask assigned to this IPv4 address entry. |
| 551 | `Device.IP.Interface.{i}.IPv4AddressNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this IP interface. |
| 552 | `Device.IP.Interface.{i}.IPv4Enable` | readWrite | boolean | Enables or disables IPv4 on this IP interface. |
| 553 | `Device.IP.Interface.{i}.IPv6Address.{i}.Alias` | readWrite | string | User-assigned alias for this IPv6 address entry. |
| 554 | `Device.IP.Interface.{i}.IPv6Address.{i}.Anycast` | readWrite | boolean | Indicates whether this IPv6 address is anycast. |
| 555 | `Device.IP.Interface.{i}.IPv6Address.{i}.Enable` | readWrite | boolean | Enables or disables this IPv6 address entry. |
| 556 | `Device.IP.Interface.{i}.IPv6Address.{i}.IPAddress` | readWrite | string | IP address associated with this IPv6 address entry. |
| 557 | `Device.IP.Interface.{i}.IPv6Address.{i}.IPAddressStatus` | readOnly | string | Current status of this IPv6 address. |
| 558 | `Device.IP.Interface.{i}.IPv6Address.{i}.Origin` | readOnly | string | Origin by which the related address or prefix was created. |
| 559 | `Device.IP.Interface.{i}.IPv6Address.{i}.PreferredLifetime` | readWrite | dateTime | Preferred lifetime for the related address or prefix. |
| 560 | `Device.IP.Interface.{i}.IPv6Address.{i}.Prefix` | readWrite | string | IP prefix associated with the related address or prefix entry. |
| 561 | `Device.IP.Interface.{i}.IPv6Address.{i}.Status` | readOnly | string | Current status of this IPv6 address entry. |
| 562 | `Device.IP.Interface.{i}.IPv6Address.{i}.ValidLifetime` | readWrite | dateTime | Valid lifetime for the related address or prefix. |
| 563 | `Device.IP.Interface.{i}.IPv6AddressNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this IP interface. |
| 564 | `Device.IP.Interface.{i}.IPv6Enable` | readWrite | boolean | Enables or disables IPv6 on this IP interface. |
| 565 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Alias` | readWrite | string | User-assigned alias for this IPv6 prefix entry. |
| 566 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Autonomous` | readWrite | boolean | Indicates whether the prefix is used for autonomous addressing. |
| 567 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.ChildPrefixBits` | readWrite | string | Child prefix bits delegated from this IPv6 prefix. |
| 568 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Enable` | readWrite | boolean | Enables or disables this IPv6 prefix entry. |
| 569 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.OnLink` | readWrite | boolean | Indicates whether the prefix is advertised as on-link. |
| 570 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Origin` | readOnly | string | Origin by which the related address or prefix was created. |
| 571 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.ParentPrefix` | readWrite | string | Parent prefix reference for this IPv6 prefix entry. |
| 572 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.PreferredLifetime` | readWrite | dateTime | Preferred lifetime for the related address or prefix. |
| 573 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Prefix` | readWrite | string | IP prefix associated with the related address or prefix entry. |
| 574 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.PrefixStatus` | readOnly | string | Current status of this IPv6 prefix. |
| 575 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.StaticType` | readWrite | string | Static type assigned to this IPv6 prefix entry. |
| 576 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Status` | readOnly | string | Current status of this IPv6 prefix entry. |
| 577 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.ValidLifetime` | readWrite | dateTime | Valid lifetime for the related address or prefix. |
| 578 | `Device.IP.Interface.{i}.IPv6PrefixNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this IP interface. |
| 579 | `Device.IP.Interface.{i}.LastChange` | readOnly | unsignedInt | Seconds since this IP interface last changed state. |
| 580 | `Device.IP.Interface.{i}.Loopback` | readWrite | boolean | Indicates whether this IP interface operates as loopback. |
| 581 | `Device.IP.Interface.{i}.LowerLayers` | readWrite | string | Lower-layer interface references for this IP interface. |
| 582 | `Device.IP.Interface.{i}.MaxMTUSize` | readWrite | unsignedInt | Maximum MTU configured for this IP interface. |
| 583 | `Device.IP.Interface.{i}.Name` | readOnly | string | Name reported for this IP interface. |
| 584 | `Device.IP.Interface.{i}.Reset` | readWrite | boolean | Triggers a reset action for this IP interface. |
| 585 | `Device.IP.Interface.{i}.Router` | readWrite | string | Router reference associated with this IP interface. |
| 586 | `Device.IP.Interface.{i}.Stats.BroadcastPacketsReceived` | readOnly | unsignedLong | Broadcast packets received on this IP interface. |
| 587 | `Device.IP.Interface.{i}.Stats.BroadcastPacketsSent` | readOnly | unsignedLong | Broadcast packets sent on this IP interface. |
| 588 | `Device.IP.Interface.{i}.Stats.BytesReceived` | readOnly | unsignedLong | Total bytes received on this IP interface. |
| 589 | `Device.IP.Interface.{i}.Stats.BytesSent` | readOnly | unsignedLong | Total bytes sent on this IP interface. |
| 590 | `Device.IP.Interface.{i}.Stats.DiscardPacketsReceived` | readOnly | unsignedInt | Received packets discarded on this IP interface. |
| 591 | `Device.IP.Interface.{i}.Stats.DiscardPacketsSent` | readOnly | unsignedInt | Outbound packets discarded on this IP interface. |
| 592 | `Device.IP.Interface.{i}.Stats.ErrorsReceived` | readOnly | unsignedInt | Receive errors seen on this IP interface. |
| 593 | `Device.IP.Interface.{i}.Stats.ErrorsSent` | readOnly | unsignedInt | Transmit errors seen on this IP interface. |
| 594 | `Device.IP.Interface.{i}.Stats.MulticastPacketsReceived` | readOnly | unsignedLong | Multicast packets received on this IP interface. |
| 595 | `Device.IP.Interface.{i}.Stats.MulticastPacketsSent` | readOnly | unsignedLong | Multicast packets sent on this IP interface. |
| 596 | `Device.IP.Interface.{i}.Stats.PacketsReceived` | readOnly | unsignedLong | Total packets received on this IP interface. |
| 597 | `Device.IP.Interface.{i}.Stats.PacketsSent` | readOnly | unsignedLong | Total packets sent on this IP interface. |
| 598 | `Device.IP.Interface.{i}.Stats.UnicastPacketsReceived` | readOnly | unsignedLong | Unicast packets received on this IP interface. |
| 599 | `Device.IP.Interface.{i}.Stats.UnicastPacketsSent` | readOnly | unsignedLong | Unicast packets sent on this IP interface. |
| 600 | `Device.IP.Interface.{i}.Stats.UnknownProtoPacketsReceived` | readOnly | unsignedInt | Packets with unknown protocol received on this IP interface. |
| 601 | `Device.IP.Interface.{i}.Status` | readOnly | string | Current status of this IP interface. |
| 602 | `Device.IP.Interface.{i}.Type` | readOnly | string | Type reported for the related object. |
| 603 | `Device.IP.Interface.{i}.ULAEnable` | readWrite | boolean | Enables or disables ULA addressing on this IP interface. |
| 604 | `Device.IP.InterfaceNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 605 | `Device.IP.ULAPrefix` | readWrite | string | Current ULA prefix configured for the device. |
| 606 | `Device.InterfaceStack.{i}.HigherLayer` | readOnly | string | Higher-layer interface reference in this stack relationship. |
| 607 | `Device.InterfaceStack.{i}.LowerLayer` | readOnly | string | Lower-layer interface reference in this stack relationship. |
| 608 | `Device.InterfaceStackNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 609 | `Device.ManagementServer.AliasBasedAddressing` | readOnly | boolean | Indicates whether alias-based addressing is supported. |
| 610 | `Device.ManagementServer.AutoCreateInstances` | readWrite | boolean | Controls automatic creation of multi-instance objects. |
| 611 | `Device.ManagementServer.CWMPRetryIntervalMultiplier` | readWrite | unsignedInt | CWMP retry timing parameter used by the management client. |
| 612 | `Device.ManagementServer.CWMPRetryMinimumWaitInterval` | readWrite | unsignedInt | CWMP retry timing parameter used by the management client. |
| 613 | `Device.ManagementServer.ConnectionRequestURL` | readOnly | string | URL used by the ACS to issue connection requests. |
| 614 | `Device.ManagementServer.ConnectionRequestUsername` | readWrite | string | Username used by the management server client. |
| 615 | `Device.ManagementServer.DefaultActiveNotificationThrottle` | readWrite | unsignedInt | Throttle interval for active notifications. |
| 616 | `Device.ManagementServer.DownloadProgressURL` | readOnly | string | URL used to report download progress. |
| 617 | `Device.ManagementServer.EnableCWMP` | readWrite | boolean | Enables or disables CWMP communication. |
| 618 | `Device.ManagementServer.InstanceMode` | readWrite | string | Instance addressing mode used by the management client. |
| 619 | `Device.ManagementServer.KickURL` | readOnly | string | Kick URL exposed by the management client. |
| 620 | `Device.ManagementServer.NATDetected` | readOnly | boolean | Indicates whether NAT is detected for ACS communication. |
| 621 | `Device.ManagementServer.ParameterKey` | readOnly | string | Parameter key associated with the most recent configuration change. |
| 622 | `Device.ManagementServer.PeriodicInformEnable` | readWrite | boolean | Enables or disables periodic Inform messages. |
| 623 | `Device.ManagementServer.PeriodicInformInterval` | readWrite | unsignedInt | Interval between periodic Inform messages, in seconds. |
| 624 | `Device.ManagementServer.PeriodicInformTime` | readWrite | dateTime | Reference time for scheduling periodic Inform messages. |
| 625 | `Device.ManagementServer.STUNEnable` | readWrite | boolean | Enables or disables STUN for connection requests. |
| 626 | `Device.ManagementServer.STUNMaximumKeepAlivePeriod` | readWrite | int | STUN keepalive timing value used by the management client. |
| 627 | `Device.ManagementServer.STUNMinimumKeepAlivePeriod` | readWrite | unsignedInt | STUN keepalive timing value used by the management client. |
| 628 | `Device.ManagementServer.STUNPassword` | readWrite | string | Shared secret or password used by the management server client. |
| 629 | `Device.ManagementServer.STUNServerAddress` | readWrite | string | STUN server address used by the management client. |
| 630 | `Device.ManagementServer.STUNServerPort` | readWrite | unsignedInt | STUN server port used by the management client. |
| 631 | `Device.ManagementServer.STUNUsername` | readWrite | string | Username used by the management server client. |
| 632 | `Device.ManagementServer.UDPConnectionRequestAddress` | readOnly | string | UDP address used for connection requests. |
| 633 | `Device.ManagementServer.URL` | readWrite | string | URL used by the management server client. |
| 634 | `Device.ManagementServer.UpgradesManaged` | readWrite | boolean | Indicates whether software upgrades are managed by the ACS. |
| 635 | `Device.ManagementServer.Username` | readWrite | string | Username used by the management server client. |
| 636 | `Device.Services.STBService.1.Capabilities.HDMI.SupportedResolutions` | readOnly | string | Display resolutions supported by the related capability or device. |
| 637 | `Device.Services.STBService.1.Capabilities.VideoDecoder.VideoStandards` | readOnly | string | Video standards supported by the decoder capability. |
| 638 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Alias` | readOnly | string | User-assigned alias for this MPEG-H Part 2 profile-level entry. |
| 639 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Level` | readOnly | string | Profile level value for this codec capability entry. |
| 640 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.MaximumDecodingCapability` | readOnly | unsignedInt | Maximum decoding capability reported for this codec entry. |
| 641 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Profile` | readOnly | string | Profile reported for the related entry. |
| 642 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevelNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the STB video decoder capability set. |
| 643 | `Device.Services.STBService.1.Components.AudioOutput.1.AudioFormat` | readOnly | string | Current audio format reported for this output. |
| 644 | `Device.Services.STBService.1.Components.AudioOutput.1.AudioLevel` | readWrite | unsignedInt | Current audio level for this output. |
| 645 | `Device.Services.STBService.1.Components.AudioOutput.1.CancelMute` | readWrite | boolean | Clears mute state for this audio output when set. |
| 646 | `Device.Services.STBService.1.Components.AudioOutput.1.Enable` | readWrite | boolean | Enables or disables this audio output. |
| 647 | `Device.Services.STBService.1.Components.AudioOutput.1.Name` | readOnly | string | Name reported for this audio output. |
| 648 | `Device.Services.STBService.1.Components.AudioOutput.1.Status` | readOnly | string | Current status of this audio output. |
| 649 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioCompression` | readWrite | string | Audio compression mode configured for this output. |
| 650 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioDB` | readWrite | string | Audio level in dB for this output. |
| 651 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioEncoding` | readWrite | string | Audio encoding mode configured for this output. |
| 652 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioGain` | readWrite | string | Audio gain setting for this output. |
| 653 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioLoopThru` | readWrite | string | Loop-through audio mode for this output. |
| 654 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioOptimalLevel` | readWrite | string | Optimal audio level setting for this output. |
| 655 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioStereoMode` | readWrite | string | Stereo mode configured for this output. |
| 656 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_MaxAudioDB` | readOnly | string | Maximum supported audio level in dB for this output. |
| 657 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_MinAudioDB` | readOnly | string | Minimum supported audio level in dB for this output. |
| 658 | `Device.Services.STBService.1.Components.AudioOutputNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the STB service. |
| 659 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.AutoLipSyncSupport` | readOnly | boolean | Indicates whether the connected display supports auto lip-sync. |
| 660 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.CECSupport` | readOnly | boolean | Indicates whether the connected display supports CEC. |
| 661 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.EEDID` | readOnly | string | EDID data reported by the connected HDMI display. |
| 662 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.HDMI3DPresent` | readOnly | boolean | Indicates whether the connected display reports HDMI 3D support. |
| 663 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.PreferredResolution` | readOnly | string | Preferred resolution reported by the connected display. |
| 664 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.Status` | readOnly | string | Current status of the connected HDMI display. |
| 665 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.SupportedResolutions` | readOnly | string | Display resolutions supported by the related capability or device. |
| 666 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.VideoLatency` | readOnly | unsignedInt | Video latency reported by the connected display. |
| 667 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.X_COMCAST-COM_EDID` | readOnly | string | EDID data reported by the connected HDMI display. |
| 668 | `Device.Services.STBService.1.Components.HDMI.1.Enable` | readWrite | boolean | Enables or disables this HDMI output. |
| 669 | `Device.Services.STBService.1.Components.HDMI.1.Name` | readOnly | string | Name reported for this HDMI output. |
| 670 | `Device.Services.STBService.1.Components.HDMI.1.ResolutionMode` | readWrite | string | Resolution selection mode for this HDMI output. |
| 671 | `Device.Services.STBService.1.Components.HDMI.1.ResolutionValue` | readWrite | string | Current resolution value for this HDMI output. |
| 672 | `Device.Services.STBService.1.Components.HDMI.1.Status` | readOnly | string | Current status of this HDMI output. |
| 673 | `Device.Services.STBService.1.Components.HDMINumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the STB service. |
| 674 | `Device.Services.STBService.1.Components.VideoDecoder.1.ContentAspectRatio` | readOnly | string | Current content aspect ratio reported by the video decoder. |
| 675 | `Device.Services.STBService.1.Components.VideoDecoder.1.Enable` | readWrite | boolean | Enables or disables this video decoder. |
| 676 | `Device.Services.STBService.1.Components.VideoDecoder.1.Name` | readOnly | string | Name reported for this video decoder. |
| 677 | `Device.Services.STBService.1.Components.VideoDecoder.1.Status` | readOnly | string | Current status of this video decoder. |
| 678 | `Device.Services.STBService.1.Components.VideoDecoder.1.X_COMCAST-COM_Standby` | readWrite | boolean | Standby state for this video decoder. |
| 679 | `Device.Services.STBService.1.Components.VideoDecoder.1.X_RDKCENTRAL-COM_MPEGHPart2` | readOnly | string | MPEG-H Part 2 capability string reported by the decoder. |
| 680 | `Device.Services.STBService.1.Components.VideoDecoderNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the STB service. |
| 681 | `Device.Services.STBService.1.Components.VideoOutput.1.AspectRatioBehaviour` | readWrite | string | Aspect-ratio handling mode for this video output. |
| 682 | `Device.Services.STBService.1.Components.VideoOutput.1.DisplayFormat` | readWrite | string | Display format configured for this video output. |
| 683 | `Device.Services.STBService.1.Components.VideoOutput.1.Enable` | readWrite | boolean | Enables or disables this video output. |
| 684 | `Device.Services.STBService.1.Components.VideoOutput.1.HDCP` | readWrite | boolean | HDCP state configured for this video output. |
| 685 | `Device.Services.STBService.1.Components.VideoOutput.1.Name` | readOnly | string | Name reported for this video output. |
| 686 | `Device.Services.STBService.1.Components.VideoOutput.1.Status` | readOnly | string | Current status of this video output. |
| 687 | `Device.Services.STBService.1.Components.VideoOutput.1.VideoFormat` | readWrite | string | Video format configured for this video output. |
| 688 | `Device.Services.STBService.1.Components.VideoOutputNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the STB service. |
| 689 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.BatteryLevelLoaded` | readOnly | unsignedInt | Battery metric reported for this RF4CE remote. |
| 690 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.BatteryLevelUnloaded` | readOnly | unsignedInt | Battery metric reported for this RF4CE remote. |
| 691 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.BatteryPercentage` | readOnly | unsignedInt | Battery metric reported for this RF4CE remote. |
| 692 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.BatteryReplacement` | readOnly | boolean | Indicates whether the RF4CE remote battery should be replaced. |
| 693 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.ImpendingDoom` | readOnly | boolean | Indicates whether the RF4CE remote reports a critical battery condition. |
| 694 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.LinkQuality` | readOnly | unsignedInt | Link quality reported for this RF4CE remote. |
| 695 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.MACAddress` | readOnly | string | MAC address associated with this RF4CE remote. |
| 696 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.NetworkAddress` | readOnly | unsignedInt | Network address reported for the related RF4CE object. |
| 697 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.RemoteId` | readOnly | unsignedInt | Remote identifier reported for this RF4CE remote. |
| 698 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.RemoteType` | readOnly | string | Remote type reported for this RF4CE remote. |
| 699 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.SignalStrength` | readOnly | int | Signal strength reported for the related entry. |
| 700 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.VersionInfoHW` | readOnly | string | Version information reported for the related RF4CE object. |
| 701 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.VersionInfoSW` | readOnly | string | Version information reported for the related RF4CE object. |
| 702 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4ceActiveChannel` | readOnly | unsignedInt | RF4CE network property reported by the subsystem. |
| 703 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4ceMACAddress` | readOnly | string | RF4CE network property reported by the subsystem. |
| 704 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4ceNetworkAddress` | readOnly | unsignedInt | RF4CE network property reported by the subsystem. |
| 705 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4cePANID` | readOnly | unsignedInt | RF4CE network property reported by the subsystem. |
| 706 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4cePairedRemotesNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the RF4CE subsystem. |
| 707 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4ceVersionInfo` | readOnly | string | Version information reported for the related RF4CE object. |
| 708 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.Capacity` | readOnly | unsignedInt | Storage capacity reported for the device. |
| 709 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.DeviceReport` | readOnly | string | Detailed health report for the storage device. |
| 710 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.FirmwareVersion` | readOnly | string | Version string reported for the eMMC flash device. |
| 711 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.LifeElapsedA` | readOnly | int | Wear indicator reported for the storage device. |
| 712 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.LifeElapsedB` | readOnly | int | Wear indicator reported for the storage device. |
| 713 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.LotID` | readOnly | string | Manufacturing lot identifier for the device. |
| 714 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.Manufacturer` | readOnly | string | Manufacturer reported for the eMMC flash device. |
| 715 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.Model` | readOnly | string | Model identifier reported for the eMMC flash device. |
| 716 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.PreEOLStateEUDA` | readOnly | string | Pre-EOL health state for the named storage area. |
| 717 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.PreEOLStateMLC` | readOnly | string | Pre-EOL health state for the named storage area. |
| 718 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.PreEOLStateSystem` | readOnly | string | Pre-EOL health state for the named storage area. |
| 719 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.ReadOnly` | readOnly | boolean | Indicates whether the device is operating in read-only mode. |
| 720 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.SerialNumber` | readOnly | string | Serial number reported for the eMMC flash device. |
| 721 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.TSBQualified` | readOnly | boolean | Indicates whether the storage device is qualified for TSB use. |
| 722 | `Device.Services.STBService.1.Enable` | readWrite | boolean | Enables or disables the STB service. |
| 723 | `Device.Services.STBServiceNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 724 | `Device.Time.ChronyEnable` | readWrite | boolean | Enables or disables Chrony-based time synchronization. |
| 725 | `Device.Time.CurrentLocalTime` | readWrite | string | Current local time reported by the device. |
| 726 | `Device.Time.Enable` | readWrite | boolean | Enables or disables the system time service. |
| 727 | `Device.Time.LocalTimeZone` | readWrite | string | Current local timezone setting reported by the device. |
| 728 | `Device.Time.NTPMaxpoll` | readWrite | unsignedInt | Chrony NTP poll interval setting. |
| 729 | `Device.Time.NTPMaxstep` | readWrite | string | Chrony maxstep setting used during large time corrections. |
| 730 | `Device.Time.NTPMinpoll` | readWrite | unsignedInt | Chrony NTP poll interval setting. |
| 731 | `Device.Time.NTPServer1` | readWrite | string | Configured NTP server address for the numbered slot. |
| 732 | `Device.Time.NTPServer1Directive` | readWrite | string | Chrony directive used for the numbered NTP server slot. |
| 733 | `Device.Time.NTPServer2` | readWrite | string | Configured NTP server address for the numbered slot. |
| 734 | `Device.Time.NTPServer2Directive` | readWrite | string | Chrony directive used for the numbered NTP server slot. |
| 735 | `Device.Time.NTPServer3` | readWrite | string | Configured NTP server address for the numbered slot. |
| 736 | `Device.Time.NTPServer3Directive` | readWrite | string | Chrony directive used for the numbered NTP server slot. |
| 737 | `Device.Time.NTPServer4` | readWrite | string | Configured NTP server address for the numbered slot. |
| 738 | `Device.Time.NTPServer4Directive` | readWrite | string | Chrony directive used for the numbered NTP server slot. |
| 739 | `Device.Time.NTPServer5` | readWrite | string | Configured NTP server address for the numbered slot. |
| 740 | `Device.Time.NTPServer5Directive` | readWrite | string | Chrony directive used for the numbered NTP server slot. |
| 741 | `Device.Time.Status` | readWrite | string | Current status of the system time service. |
| 742 | `Device.Time.X_RDK_CurrentUTCTime` | readOnly | string | Current UTC time reported by the device. |
| 743 | `Device.WiFi.AccessPoint.{i}.Alias` | readWrite | string | User-assigned alias for this Wi-Fi access point. |
| 744 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.Active` | readOnly | boolean | Indicates whether the related entry is currently active. |
| 745 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.AuthenticationState` | readOnly | boolean | Configuration or status value for this associated Wi-Fi client. |
| 746 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.LastDataDownlinkRate` | readOnly | unsignedInt | Configuration or status value for this associated Wi-Fi client. |
| 747 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.LastDataUplinkRate` | readOnly | unsignedInt | Configuration or status value for this associated Wi-Fi client. |
| 748 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.MACAddress` | readOnly | string | MAC address associated with this associated Wi-Fi client. |
| 749 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.Retransmissions` | readOnly | unsignedInt | Configuration or status value for this associated Wi-Fi client. |
| 750 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.SignalStrength` | readOnly | int | Signal strength reported for the related entry. |
| 751 | `Device.WiFi.AccessPoint.{i}.AssociatedDeviceNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this Wi-Fi access point. |
| 752 | `Device.WiFi.AccessPoint.{i}.Enable` | readWrite | boolean | Enables or disables this Wi-Fi access point. |
| 753 | `Device.WiFi.AccessPoint.{i}.RetryLimit` | readWrite | unsignedInt | Configuration or status value for this Wi-Fi access point. |
| 754 | `Device.WiFi.AccessPoint.{i}.SSIDAdvertisementEnabled` | readWrite | boolean | Controls whether this access point advertises its SSID. |
| 755 | `Device.WiFi.AccessPoint.{i}.SSIDReference` | readWrite | string | Reference to the SSID object used by this entry. |
| 756 | `Device.WiFi.AccessPoint.{i}.Security.KeyPassphrase` | readWrite | string | Passphrase configured for the related Wi-Fi profile. |
| 757 | `Device.WiFi.AccessPoint.{i}.Security.ModeEnabled` | readWrite | string | Security mode currently enabled for the related Wi-Fi object. |
| 758 | `Device.WiFi.AccessPoint.{i}.Security.ModesSupported` | readOnly | string | Security modes supported by the related Wi-Fi object. |
| 759 | `Device.WiFi.AccessPoint.{i}.Security.PreSharedKey` | readWrite | hexBinary | Pre-shared key configured for the related Wi-Fi object. |
| 760 | `Device.WiFi.AccessPoint.{i}.Security.RadiusSecret` | readWrite | string | Shared secret or password used by this Wi-Fi access point. |
| 761 | `Device.WiFi.AccessPoint.{i}.Security.RadiusServerIPAddr` | readWrite | string | RADIUS server IP address used by this access point. |
| 762 | `Device.WiFi.AccessPoint.{i}.Security.RadiusServerPort` | readWrite | unsignedInt | Port value used by this Wi-Fi access point. |
| 763 | `Device.WiFi.AccessPoint.{i}.Security.RekeyingInterval` | readWrite | unsignedInt | Key rekey interval for this Wi-Fi security profile. |
| 764 | `Device.WiFi.AccessPoint.{i}.Security.WEPKey` | readWrite | hexBinary | WEP key configured for the related Wi-Fi object. |
| 765 | `Device.WiFi.AccessPoint.{i}.Status` | readOnly | string | Current status of this Wi-Fi access point. |
| 766 | `Device.WiFi.AccessPoint.{i}.UAPSDCapability` | readOnly | boolean | U-APSD capability or enable state for this access point. |
| 767 | `Device.WiFi.AccessPoint.{i}.UAPSDEnable` | readWrite | boolean | U-APSD capability or enable state for this access point. |
| 768 | `Device.WiFi.AccessPoint.{i}.WMMCapability` | readOnly | boolean | WMM capability or enable state for this access point. |
| 769 | `Device.WiFi.AccessPoint.{i}.WMMEnable` | readWrite | boolean | WMM capability or enable state for this access point. |
| 770 | `Device.WiFi.AccessPoint.{i}.WPS.ConfigMethodsEnabled` | readWrite | string | WPS configuration methods enabled or supported for the related Wi-Fi object. |
| 771 | `Device.WiFi.AccessPoint.{i}.WPS.ConfigMethodsSupported` | readOnly | string | WPS configuration methods enabled or supported for the related Wi-Fi object. |
| 772 | `Device.WiFi.AccessPoint.{i}.WPS.Enable` | readWrite | boolean | Enables or disables this Wi-Fi access point. |
| 773 | `Device.WiFi.AccessPointNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 774 | `Device.WiFi.EndPoint.{i}.Alias` | readWrite | string | User-assigned alias for this Wi-Fi endpoint. |
| 775 | `Device.WiFi.EndPoint.{i}.Enable` | readWrite | boolean | Enables or disables this Wi-Fi endpoint. |
| 776 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Alias` | readWrite | string | User-assigned alias for this Wi-Fi endpoint profile. |
| 777 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Enable` | readWrite | boolean | Enables or disables this Wi-Fi endpoint profile. |
| 778 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Location` | readWrite | string | Location hint associated with this Wi-Fi endpoint profile. |
| 779 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Priority` | readWrite | unsignedInt | Scheduling priority for this Wi-Fi endpoint profile. |
| 780 | `Device.WiFi.EndPoint.{i}.Profile.{i}.SSID` | readWrite | string | SSID string used by the related Wi-Fi object. |
| 781 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Security.KeyPassphrase` | readWrite | string | Passphrase configured for the related Wi-Fi profile. |
| 782 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Security.ModeEnabled` | readWrite | string | Security mode currently enabled for the related Wi-Fi object. |
| 783 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Security.PreSharedKey` | readWrite | hexBinary | Pre-shared key configured for the related Wi-Fi object. |
| 784 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Security.WEPKey` | readWrite | hexBinary | WEP key configured for the related Wi-Fi object. |
| 785 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Status` | readOnly | string | Current status of this Wi-Fi endpoint profile. |
| 786 | `Device.WiFi.EndPoint.{i}.ProfileNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this Wi-Fi endpoint. |
| 787 | `Device.WiFi.EndPoint.{i}.ProfileReference` | readWrite | string | Reference to the active Wi-Fi endpoint profile. |
| 788 | `Device.WiFi.EndPoint.{i}.SSIDReference` | readOnly | string | Reference to the SSID object used by this entry. |
| 789 | `Device.WiFi.EndPoint.{i}.Security.ModesEnabled` | readOnly | string | Security mode currently enabled for the related Wi-Fi object. |
| 790 | `Device.WiFi.EndPoint.{i}.Security.ModesSupported` | readOnly | string | Security modes supported by the related Wi-Fi object. |
| 791 | `Device.WiFi.EndPoint.{i}.Stats.LastDataDownlinkRate` | readOnly | unsignedInt | Most recent downlink data rate for this Wi-Fi endpoint. |
| 792 | `Device.WiFi.EndPoint.{i}.Stats.LastDataUplinkRate` | readOnly | unsignedInt | Most recent uplink data rate for this Wi-Fi endpoint. |
| 793 | `Device.WiFi.EndPoint.{i}.Stats.Retransmissions` | readOnly | unsignedInt | Retransmission count observed for this Wi-Fi endpoint. |
| 794 | `Device.WiFi.EndPoint.{i}.Stats.SignalStrength` | readOnly | int | Reported signal strength for this Wi-Fi endpoint. |
| 795 | `Device.WiFi.EndPoint.{i}.Status` | readOnly | string | Current status of this Wi-Fi endpoint. |
| 796 | `Device.WiFi.EndPoint.{i}.WPS.ConfigMethodsEnabled` | readWrite | string | WPS configuration methods enabled or supported for the related Wi-Fi object. |
| 797 | `Device.WiFi.EndPoint.{i}.WPS.ConfigMethodsSupported` | readOnly | string | WPS configuration methods enabled or supported for the related Wi-Fi object. |
| 798 | `Device.WiFi.EndPoint.{i}.WPS.Enable` | readWrite | boolean | Enables or disables this Wi-Fi endpoint. |
| 799 | `Device.WiFi.EndPointNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 800 | `Device.WiFi.Radio.{i}.OperatingChannelBandwidth` | readOnly | string | Current operating channel bandwidth of this Wi-Fi radio. |
| 801 | `Device.WiFi.Radio.{i}.Stats.Noise` | readOnly | int | Reported noise floor for this Wi-Fi radio. |
| 802 | `Device.WiFi.Radio.{i}.Stats.PacketsReceived` | readOnly | unsignedLong | Total packets received on this Wi-Fi radio. |
| 803 | `Device.WiFi.RadioNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 804 | `Device.WiFi.SSID.{i}.Alias` | readWrite | string | User-assigned alias for this SSID interface. |
| 805 | `Device.WiFi.SSID.{i}.BSSID` | readOnly | string | BSSID reported for this SSID interface. |
| 806 | `Device.WiFi.SSID.{i}.Enable` | readWrite | boolean | Enables or disables this SSID interface. |
| 807 | `Device.WiFi.SSID.{i}.LastChange` | readOnly | unsignedInt | Seconds since this SSID interface last changed state. |
| 808 | `Device.WiFi.SSID.{i}.LowerLayers` | readWrite | string | Lower-layer interface references for this SSID interface. |
| 809 | `Device.WiFi.SSID.{i}.MACAddress` | readOnly | string | MAC address associated with this SSID interface. |
| 810 | `Device.WiFi.SSID.{i}.Name` | readOnly | string | Name reported for this SSID interface. |
| 811 | `Device.WiFi.SSID.{i}.SSID` | readWrite | string | SSID string used by the related Wi-Fi object. |
| 812 | `Device.WiFi.SSID.{i}.Stats.BroadcastPacketsReceived` | readOnly | unsignedLong | Broadcast packets received on this SSID interface. |
| 813 | `Device.WiFi.SSID.{i}.Stats.BroadcastPacketsSent` | readOnly | unsignedLong | Broadcast packets sent on this SSID interface. |
| 814 | `Device.WiFi.SSID.{i}.Stats.BytesReceived` | readOnly | unsignedLong | Total bytes received on this SSID interface. |
| 815 | `Device.WiFi.SSID.{i}.Stats.BytesSent` | readOnly | unsignedLong | Total bytes sent on this SSID interface. |
| 816 | `Device.WiFi.SSID.{i}.Stats.DiscardPacketsReceived` | readOnly | unsignedInt | Received packets discarded on this SSID interface. |
| 817 | `Device.WiFi.SSID.{i}.Stats.DiscardPacketsSent` | readOnly | unsignedInt | Outbound packets discarded on this SSID interface. |
| 818 | `Device.WiFi.SSID.{i}.Stats.ErrorsReceived` | readOnly | unsignedInt | Receive errors seen on this SSID interface. |
| 819 | `Device.WiFi.SSID.{i}.Stats.ErrorsSent` | readOnly | unsignedInt | Transmit errors seen on this SSID interface. |
| 820 | `Device.WiFi.SSID.{i}.Stats.MulticastPacketsReceived` | readOnly | unsignedLong | Multicast packets received on this SSID interface. |
| 821 | `Device.WiFi.SSID.{i}.Stats.MulticastPacketsSent` | readOnly | unsignedLong | Multicast packets sent on this SSID interface. |
| 822 | `Device.WiFi.SSID.{i}.Stats.PacketsReceived` | readOnly | unsignedLong | Total packets received on this SSID interface. |
| 823 | `Device.WiFi.SSID.{i}.Stats.PacketsSent` | readOnly | unsignedLong | Total packets sent on this SSID interface. |
| 824 | `Device.WiFi.SSID.{i}.Stats.UnicastPacketsReceived` | readOnly | unsignedLong | Unicast packets received on this SSID interface. |
| 825 | `Device.WiFi.SSID.{i}.Stats.UnicastPacketsSent` | readOnly | unsignedLong | Unicast packets sent on this SSID interface. |
| 826 | `Device.WiFi.SSID.{i}.Stats.UnknownProtoPacketsReceived` | readOnly | unsignedInt | Packets with unknown protocol received on this SSID interface. |
| 827 | `Device.WiFi.SSID.{i}.Status` | readOnly | string | Current status of this SSID interface. |
| 828 | `Device.WiFi.SSIDNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 829 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.80211kvrEnable` | readWrite | boolean | Enables or disables 802.11k/v/r roaming support. |
| 830 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.Enable` | readWrite | boolean | Enables or disables the Wi-Fi client roaming policy. |
| 831 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_APcontrolBeaconsMissedTime` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 832 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_APcontrolThresholdLevel` | readWrite | int | Band-steering threshold or control used by the client roaming policy. |
| 833 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_APcontrolTimeframe` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 834 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BackOffTime` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 835 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BestDeltaLevelConnected` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 836 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BestDeltaLevelDisconnected` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 837 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerBeaconsMissedTime` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 838 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerThresholdLevel` | readWrite | int | Band-steering threshold or control used by the client roaming policy. |
| 839 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerTimeframe` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 840 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PreAssn_BestDeltaLevel` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 841 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PreAssn_BestThresholdLevel` | readWrite | int | Band-steering threshold or control used by the client roaming policy. |
| 842 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.SelfSteer_OverrideEnable` | readWrite | boolean | Band-steering threshold or control used by the client roaming policy. |
| 843 | `Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable` | readWrite | boolean | Master enable for the Wi-Fi subsystem. |
| 844 | `Device.X_COMCAST-COM_Xcalibur.Client.SecuritySystem.ssDeviceId` | readOnly | string | Security system device identifier. |
| 845 | `Device.X_COMCAST-COM_Xcalibur.Client.SecuritySystem.ssDeviceReg` | readOnly | dateTime | Security system device registration time. |
| 846 | `Device.X_COMCAST-COM_Xcalibur.Client.SecuritySystem.ssErrorCnt` | readOnly | unsignedInt | Security system error count. |
| 847 | `Device.X_COMCAST-COM_Xcalibur.Client.SecuritySystem.ssRegTs` | readOnly | boolean | Indicates whether a security system registration timestamp is available. |
| 848 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreAppId` | readOnly | string | Application identifier for this XRE connection entry. |
| 849 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnEstTs` | readOnly | string | Connection establishment timestamp for this XRE connection entry. |
| 850 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnIfName` | readOnly | string | Interface name used by this XRE connection entry. |
| 851 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnRetryAttempts` | readOnly | unsignedInt | Retry attempts recorded for this XRE connection entry. |
| 852 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnStatus` | readOnly | string | Current status of this XRE connection entry. |
| 853 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnURL` | readOnly | string | Connection URL used by this XRE connection entry. |
| 854 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreAvgCmdProcTime` | readOnly | int | Command processing time statistic reported by XRE. |
| 855 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreChannelMapId` | readOnly | string | Channel map identifier currently used by the XRE client. |
| 856 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreCommandCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 857 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreControllerId` | readOnly | string | Controller identifier reported by the XRE client. |
| 858 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreEnable` | readWrite | boolean | Enables or disables the XRE client. |
| 859 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreErrorCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 860 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreFlushLocalCache` | readWrite | boolean | Triggers an XRE local cache flush when set. |
| 861 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreGatewaySTBMAC` | readOnly | string | Gateway STB MAC address reported by the XRE client. |
| 862 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreGetTWPDiags` | readOnly | string | Diagnostic payload returned by XRE TWP diagnostics. |
| 863 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreLastURLAccessed` | readOnly | string | Last URL accessed by the XRE client. |
| 864 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreLastVideoUrl` | readOnly | string | Last video URL accessed by the XRE client. |
| 865 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreLogLevel` | readWrite | string | Logging level used by the XRE client. |
| 866 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreMaxCmdProcTime` | readOnly | int | Command processing time statistic reported by XRE. |
| 867 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreMinCmdProcTime` | readOnly | int | Command processing time statistic reported by XRE. |
| 868 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xrePlantId` | readOnly | string | Plant identifier reported by the XRE client. |
| 869 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreReceiverId` | readOnly | string | Receiver identifier reported by the XRE client. |
| 870 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreRefreshXreSession` | readWrite | boolean | Triggers XRE session refresh behavior. |
| 871 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreRefreshXreSessionWithRR` | readWrite | int | Controls refresh-with-RR behavior for the XRE session. |
| 872 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreSessionId` | readOnly | string | Active XRE session identifier reported by the client. |
| 873 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreSessionLastModTs` | readOnly | string | Timestamp of the last XRE session update. |
| 874 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreSessionUptime` | readOnly | string | Uptime of the current XRE session. |
| 875 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreStatus` | readOnly | string | Configuration or status value for the XRE client. |
| 876 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotAnimCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 877 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotAppCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 878 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotFlashCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 879 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotFontCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 880 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotHtmlTxtCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 881 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotImgCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 882 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotNineSliceImgCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 883 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotRectCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 884 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotSoundCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 885 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotStyleshtCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 886 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotTxtCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 887 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotTxtIpCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 888 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotVideoCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 889 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotViewCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 890 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreVersion` | readOnly | string | Version string reported by the XRE client. |
| 891 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreVodId` | readOnly | string | VOD identifier reported by the XRE client. |
| 892 | `Device.X_COMCAST-COM_Xcalibur.Client.xconfCheckNow` | readWrite | string | Triggers an immediate Xconf check for the Xcalibur client. |
| 893 | `Device.X_COMCAST-COM_Xcalibur.DevApp.devAppNumAps` | readOnly | unsignedInt | Number of DevApp application entries reported by the platform. |
| 894 | `Device.X_COMCAST-COM_Xcalibur.DevApp.devAppTable.{i}.devAppId` | readOnly | string | Application identifier for this DevApp entry. |
| 895 | `Device.X_COMCAST-COM_Xcalibur.DevApp.devAppTable.{i}.devAppRestartCapability` | readOnly | string | Restart capability reported for this DevApp entry. |
| 896 | `Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewayDeviceFriendlyName` | readOnly | string | Gateway identification value reported by TRM. |
| 897 | `Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewayMoCAIP` | readOnly | string | Gateway identification value reported by TRM. |
| 898 | `Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewayMoCAMAC` | readOnly | string | Gateway identification value reported by TRM. |
| 899 | `Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewaySTBMAC` | readOnly | string | Gateway identification value reported by TRM. |
| 900 | `Device.X_RDKCENTRAL-COM_T2.ReportProfiles` | readWrite | string | Telemetry 2.0 report profiles payload. |
| 901 | `Device.X_RDKCENTRAL-COM_T2.ReportProfilesMsgPack` | readWrite | string | Telemetry 2.0 report profiles payload. |
| 902 | `Device.X_RDK_WebPA_DNSText.URL` | readWrite | string | Bootstrap URL used to retrieve WebPA DNS text records. |
| 903 | `Device.X_RDK_WebPA_Server.URL` | readOnly | string | Current WebPA server URL from the bootstrap store. |
| 904 | `Device.X_RDK_WebPA_TokenServer.URL` | readOnly | string | Current WebPA token server URL from the bootstrap store. |