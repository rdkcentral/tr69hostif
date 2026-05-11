# TR-181 Datamodel Parameter List

> **Automation Test Suite Reference** - Generated from XML data-model files in `src/hostif/parodusClient/waldb/data-model/` (TR-181 v2.12, Comcast/RDK)

| Access | Count | Test Action |
|--------|-------|-------------|
| readOnly  | 365 | GET only - assert non-empty or expected value |
| readWrite | 570 | GET + SET - verify set/get round-trip |
| writeOnly | 21 | SET only - verify accepted without error |
| (unspecified) | 1 | Verify behavior per component |
| **Total** | **957** | |

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
| 65 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RDKDownloadManager.InstallPackage` | writeOnly | string | RDK download manager value for InstallPackage. |
| 66 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RDKVersion` | readOnly | string | RDK software version reported by the platform. |
| 67 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.AuthService.Host` | readWrite | string | Host value used by RFC setting AuthService Host. |
| 68 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDB` | readWrite | boolean | Control flag used by RFC setting Bootstrap Control ClearDB. |
| 69 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDBEnd` | readWrite | boolean | Control flag used by RFC setting Bootstrap Control ClearDBEnd. |
| 70 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.NetflixESNprefix` | readWrite | string | RFC configuration value for Bootstrap NetflixESNprefix. |
| 71 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.OsClass` | readWrite | string | RFC configuration value for Bootstrap OsClass. |
| 72 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName` | readWrite | string | RFC configuration value for Bootstrap PartnerName. |
| 73 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName` | readWrite | string | RFC configuration value for Bootstrap PartnerProductName. |
| 74 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.SsrUrl` | readWrite | string | Endpoint URL used by RFC setting Bootstrap SsrUrl. |
| 75 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.XconfRecoveryUrl` | readWrite | string | Endpoint URL used by RFC setting Bootstrap XconfRecoveryUrl. |
| 76 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.XconfUrl` | readWrite | string | Endpoint URL used by RFC setting Bootstrap XconfUrl. |
| 77 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.ClearParam` | readWrite | string | RFC configuration value for ClearParam. |
| 78 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CloudStore.Uri` | readWrite | string | Endpoint URL used by RFC setting CloudStore Uri. |
| 79 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB` | readWrite | boolean | Control flag used by RFC setting Control ClearDB. |
| 80 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDBEnd` | readWrite | boolean | Control flag used by RFC setting Control ClearDBEnd. |
| 81 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ConfigChangeTime` | readWrite | unsignedInt | RFC configuration value for Control ConfigChangeTime. |
| 82 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ConfigSetHash` | readWrite | string | RFC configuration value for Control ConfigSetHash. |
| 83 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ConfigSetTime` | readWrite | unsignedInt | RFC configuration value for Control ConfigSetTime. |
| 84 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow` | readWrite | unsignedInt | Control flag used by RFC setting Control RetrieveNow. |
| 85 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfSelector` | readWrite | string | RFC configuration value for Control XconfSelector. |
| 86 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.XconfUrl` | readWrite | string | Endpoint URL used by RFC setting Control XconfUrl. |
| 87 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.S3BucketUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload S3BucketUrl. |
| 88 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.S3SigningUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload S3SigningUrl. |
| 89 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.crashPortalDEVUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload crashPortalDEVUrl. |
| 90 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.crashPortalPRODUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload crashPortalPRODUrl. |
| 91 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.crashPortalSTBUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload crashPortalSTBUrl. |
| 92 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.CrashUpload.crashPortalVBNUrl` | readWrite | string | Endpoint URL used by RFC setting CrashUpload crashPortalVBNUrl. |
| 93 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DAC.ConfigURL` | readWrite | string | Endpoint URL used by RFC setting DAC ConfigURL. |
| 94 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DAC.dacBundleFirmwareCompatibilityKey` | readWrite | string | RFC configuration value for DAC dacBundleFirmwareCompatibilityKey. |
| 95 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DAC.dacBundlePlatformName` | readWrite | string | RFC configuration value for DAC dacBundlePlatformName. |
| 96 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DisplaySettings.AudioPort.HDMI.Enabled` | readWrite | boolean | RFC flag that enables or disables AudioPort HDMI. |
| 97 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DisplaySettings.AudioPort.HDMI_ARC.Enabled` | readWrite | boolean | RFC flag that enables or disables AudioPort HDMI_ARC. |
| 98 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DisplaySettings.AudioPort.SPDIF.Enabled` | readWrite | boolean | RFC flag that enables or disables AudioPort SPDIF. |
| 99 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DisplaySettings.AudioPort.SPEAKER.Enabled` | readWrite | boolean | RFC flag that enables or disables AudioPort SPEAKER. |
| 100 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DynamicIUIUpdate.IUISelector` | readWrite | string | RFC configuration value for DynamicIUIUpdate IUISelector. |
| 101 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DynamicIUIUpdate.Pathway.Uri` | readWrite | string | Endpoint URL used by RFC setting DynamicIUIUpdate Pathway Uri. |
| 102 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.1080pGraphics.Enable` | readWrite | boolean | RFC flag that enables or disables Feature 1080pGraphics Enable. |
| 103 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AAMP_CFG.DashPlaybackExclusions` | readWrite | string | RFC configuration value for Feature AAMP CFG DashPlaybackExclusions. |
| 104 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AAMP_CFG.DashPlaybackInclusions` | readWrite | string | RFC configuration value for Feature AAMP CFG DashPlaybackInclusions. |
| 105 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AAMP_CFG.b64Config` | readWrite | string | RFC configuration value for Feature AAMP CFG b64Config. |
| 106 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID` | readWrite | string | RFC configuration value for Feature AccountInfo AccountID. |
| 107 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Airplay.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Airplay Enable. |
| 108 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AmazonPrimeVideo.SyePlayer.Enable` | readWrite | boolean | RFC flag that enables or disables Feature AmazonPrimeVideo SyePlayer Enable. |
| 109 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppConfig.App.{i}.AppName` | readWrite | string | RFC configuration value for Feature AppConfig App entry AppName. |
| 110 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppConfig.App.{i}.AppSize` | readWrite | string | RFC configuration value for Feature AppConfig App entry AppSize. |
| 111 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppConfig.App.{i}.AppUrl` | readWrite | string | Endpoint URL used by RFC setting Feature AppConfig App entry AppUrl. |
| 112 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppConfig.AppCnt` | readWrite | unsignedInt | RFC configuration value for Feature AppConfig AppCnt. |
| 113 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppConfig.Enable` | readWrite | boolean | RFC flag that enables or disables Feature AppConfig Enable. |
| 114 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppHibernate.DelayInSecLaunchedToSuspended` | readWrite | unsignedInt | RFC configuration value for Feature AppHibernate DelayInSecLaunchedToSuspended. |
| 115 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppHibernate.DelayInSecResumedToSuspended` | readWrite | unsignedInt | RFC configuration value for Feature AppHibernate DelayInSecResumedToSuspended. |
| 116 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppHibernate.Enable` | readWrite | boolean | RFC flag that enables or disables Feature AppHibernate Enable. |
| 117 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AutoReboot.Enable` | readWrite | boolean | RFC flag that enables or disables Feature AutoReboot Enable. |
| 118 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AutoReboot.fwDelayReboot` | readWrite | int | RFC configuration value for Feature AutoReboot fwDelayReboot. |
| 119 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTR.AudioIn.Enable` | readWrite | boolean | RFC flag that enables or disables Feature BTR AudioIn Enable. |
| 120 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTR.DebugMode.Enable` | readWrite | boolean | RFC flag that enables or disables Feature BTR DebugMode Enable. |
| 121 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTR.GamePad.Enable` | readWrite | boolean | RFC flag that enables or disables Feature BTR GamePad Enable. |
| 122 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTSplitAudio.Enable` | readWrite | boolean | RFC flag that enables or disables Feature BTSplitAudio Enable. |
| 123 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTSplitAudio.Language` | readWrite | string | RFC configuration value for Feature BTSplitAudio Language. |
| 124 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BootstrapConfig.Enable` | readWrite | boolean | RFC flag that enables or disables Feature BootstrapConfig Enable. |
| 125 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CDLDM.CDLModuleUrl` | readWrite | string | Endpoint URL used by RFC setting Feature CDLDM CDLModuleUrl. |
| 126 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CPC1960.Enable` | readWrite | boolean | RFC flag that enables or disables Feature CPC1960 Enable. |
| 127 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CRL.DirectOCSP` | readWrite | boolean | RFC configuration value for Feature CRL DirectOCSP. |
| 128 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CRL.Enable` | readWrite | boolean | RFC flag that enables or disables Feature CRL Enable. |
| 129 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpEnd` | readWrite | int | RFC configuration value for Feature Canary wakeUpEnd. |
| 130 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Canary.wakeUpStart` | readWrite | int | RFC configuration value for Feature Canary wakeUpStart. |
| 131 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CodeBigFirst.Enable` | writeOnly | boolean | RFC flag that enables or disables CodeBigFirst. |
| 132 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CrashportalEndpoint.Enable` | readWrite | string | RFC flag that enables or disables Feature CrashportalEndpoint Enable. |
| 133 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CrashportalEndpoint.URL` | readWrite | string | Endpoint URL used by RFC setting Feature CrashportalEndpoint URL. |
| 134 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CredDwnld.Enable` | readWrite | string | RFC flag that enables or disables Feature CredDwnld Enable. |
| 135 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CredDwnld.Use` | readWrite | string | RFC configuration value for Feature CredDwnld Use. |
| 136 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable` | readWrite | boolean | RFC flag that enables or disables Feature DAB Enable. |
| 137 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DHCPv6Client.Enable` | readWrite | boolean | RFC flag that enables or disables Feature DHCPv6Client Enable. |
| 138 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DNSStrictOrder.Enable` | readWrite | boolean | RFC flag that enables or disables Feature DNSStrictOrder Enable. |
| 139 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DefaultSupportedLocales` | readWrite | string | RFC configuration value for Feature DefaultSupportedLocales. |
| 140 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DolbyVision.Enable` | writeOnly | boolean | RFC flag that enables or disables DolbyVision. |
| 141 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DtmAnalytics.Enable` | readWrite | string | RFC flag that enables or disables Feature DtmAnalytics Enable. |
| 142 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DtmAnalytics.RecordsToBuffer` | readWrite | string | RFC configuration value for Feature DtmAnalytics RecordsToBuffer. |
| 143 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DtmAnalytics.ServiceUrl` | readWrite | string | Endpoint URL used by RFC setting Feature DtmAnalytics ServiceUrl. |
| 144 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.EnableHttpCDL.Enable` | readWrite | boolean | RFC flag that enables or disables Feature EnableHttpCDL Enable. |
| 145 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.EncryptCloudUpload.Enable` | writeOnly | boolean | RFC flag that enables or disables EncryptCloudUpload. |
| 146 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.EntOsAppPlatform.EnableDistPlatform` | readWrite | boolean | RFC configuration value for Feature EntOsAppPlatform EnableDistPlatform. |
| 147 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FOG_CFG.b64Config` | readWrite | string | RFC configuration value for Feature FOG CFG b64Config. |
| 148 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.Enable` | readWrite | boolean | RFC flag that enables or disables Feature FWUpdate AutoExcluded Enable. |
| 149 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.FWUpdate.AutoExcluded.XconfUrl` | readWrite | string | Endpoint URL used by RFC setting Feature FWUpdate AutoExcluded XconfUrl. |
| 150 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HdmiCecSink.CECVersion` | readWrite | string | RFC configuration value for Feature HdmiCecSink CECVersion. |
| 151 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IDS.Enable` | readWrite | boolean | RFC flag that enables or disables Feature IDS Enable. |
| 152 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IDS.ScanTask` | readWrite | string | RFC configuration value for Feature IDS ScanTask. |
| 153 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IPControl.Service.Discovery.Enable` | readWrite | boolean | RFC flag that enables or disables Feature IPControl Service Discovery Enable. |
| 154 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IPControl.Subsystem.RICS.Enable` | readWrite | boolean | RFC flag that enables or disables Feature IPControl Subsystem RICS Enable. |
| 155 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IPRemotePort.Enable` | readWrite | boolean | RFC flag that enables or disables Feature IPRemotePort Enable. |
| 156 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.InactiveApplications.Maximum` | readWrite | unsignedInt | RFC configuration value for Feature InactiveApplications Maximum. |
| 157 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IncrementalCDL.Enable` | readWrite | boolean | RFC flag that enables or disables Feature IncrementalCDL Enable. |
| 158 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.JSPPCache.Enable` | readWrite | boolean | RFC flag that enables or disables Feature JSPPCache Enable. |
| 159 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.AdCacheEnable` | readWrite | string | RFC configuration value for Feature LSA AdCacheEnable. |
| 160 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Advt_Opt_Out_Key` | readWrite | string | RFC configuration value for Feature LSA Advt Opt Out Key. |
| 161 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.AltconReceiver` | readWrite | string | RFC configuration value for Feature LSA AltconReceiver. |
| 162 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.ByteRangeDownload` | readWrite | string | RFC configuration value for Feature LSA ByteRangeDownload. |
| 163 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Enable` | readWrite | string | RFC flag that enables or disables Feature LSA Enable. |
| 164 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.PSNUrl` | readWrite | string | Endpoint URL used by RFC setting Feature LSA PSNUrl. |
| 165 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.PlacementReqUrl` | readWrite | string | Endpoint URL used by RFC setting Feature LSA PlacementReqUrl. |
| 166 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.ProgrammerEnable` | readWrite | string | RFC configuration value for Feature LSA ProgrammerEnable. |
| 167 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Schema_Location` | readWrite | string | RFC configuration value for Feature LSA Schema Location. |
| 168 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Schemas_NGOD` | readWrite | string | RFC configuration value for Feature LSA Schemas NGOD. |
| 169 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Schemas_admin` | readWrite | string | RFC configuration value for Feature LSA Schemas admin. |
| 170 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Schemas_core` | readWrite | string | RFC configuration value for Feature LSA Schemas core. |
| 171 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.XMLNS_Schema` | readWrite | string | RFC configuration value for Feature LSA XMLNS Schema. |
| 172 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.XMLSchema_Instance` | readWrite | string | RFC configuration value for Feature LSA XMLSchema Instance. |
| 173 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Xifaid_Xml_Key` | readWrite | string | RFC configuration value for Feature LSA Xifaid Xml Key. |
| 174 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LaunchDarkly.EnvKeyLabel` | readWrite | string | RFC configuration value for Feature LaunchDarkly EnvKeyLabel. |
| 175 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LnFUseXPKI.Enable` | readWrite | boolean | RFC flag that enables or disables Feature LnFUseXPKI Enable. |
| 176 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LogUploadBeforeDeepSleep.Enable` | readWrite | boolean | RFC flag that enables or disables Feature LogUploadBeforeDeepSleep Enable. |
| 177 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LogUploadEndpoint.Enable` | readWrite | string | RFC flag that enables or disables Feature LogUploadEndpoint Enable. |
| 178 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LogUploadEndpoint.URL` | readWrite | string | Endpoint URL used by RFC setting Feature LogUploadEndpoint URL. |
| 179 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LoudnessEquivalence.Enable` | readWrite | boolean | RFC flag that enables or disables Feature LoudnessEquivalence Enable. |
| 180 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MEMSWAP.Enable` | readWrite | string | RFC flag that enables or disables Feature MEMSWAP Enable. |
| 181 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable` | readWrite | boolean | RFC flag that enables or disables Feature MOCASSH Enable. |
| 182 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DAPv2_Enable` | readWrite | boolean | RFC configuration value for Feature MS12 DAPv2 Enable. |
| 183 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DE_Enable` | readWrite | boolean | RFC configuration value for Feature MS12 DE Enable. |
| 184 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MTLS.mTlsXcSsr.Enable` | readWrite | boolean | RFC flag that enables or disables Feature MTLS mTlsXcSsr Enable. |
| 185 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ManageableNotification.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ManageableNotification Enable. |
| 186 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Miracast.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Miracast Enable. |
| 187 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NTP.failoverServer` | readWrite | string | RFC configuration value for Feature NTP failoverServer. |
| 188 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonPersistent.WebPACDL.Enable` | readWrite | boolean | RFC flag that enables or disables Feature NonPersistent WebPACDL Enable. |
| 189 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.ApparmorBlocklist` | readWrite | string | RFC configuration value for Feature NonRootSupport ApparmorBlocklist. |
| 190 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.Blocklist` | readWrite | string | RFC configuration value for Feature NonRootSupport Blocklist. |
| 191 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonRootSupport.Enable` | readWrite | boolean | RFC flag that enables or disables Feature NonRootSupport Enable. |
| 192 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PeriodicFWCheck.Enable` | readWrite | boolean | RFC flag that enables or disables Feature PeriodicFWCheck Enable. |
| 193 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PeriodicFwCheck.timeperiod` | readWrite | unsignedInt | RFC configuration value for Feature PeriodicFwCheck timeperiod. |
| 194 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PingTelemetry.BurstCnt` | readWrite | unsignedInt | RFC configuration value for Feature PingTelemetry BurstCnt. |
| 195 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PingTelemetry.Enable` | readWrite | boolean | RFC flag that enables or disables Feature PingTelemetry Enable. |
| 196 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PingTelemetry.EndTime` | readWrite | unsignedInt | RFC configuration value for Feature PingTelemetry EndTime. |
| 197 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PingTelemetry.StartTime` | readWrite | unsignedInt | RFC configuration value for Feature PingTelemetry StartTime. |
| 198 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PingTelemetry.Type` | readWrite | string | RFC configuration value for Feature PingTelemetry Type. |
| 199 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Power.DeepSleepNotification.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Power DeepSleepNotification Enable. |
| 200 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Power.PwrMgr2.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Power PwrMgr2 Enable. |
| 201 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Power.UserInactivityNotification.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Power UserInactivityNotification Enable. |
| 202 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Power.UserInactivityNotification.TimeMinutes` | readWrite | unsignedInt | RFC configuration value for Feature Power UserInactivityNotification TimeMinutes. |
| 203 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Preference.URL` | readWrite | string | Endpoint URL used by RFC setting Feature Preference URL. |
| 204 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PressAndRelease.EOSMethod` | readWrite | int | RFC configuration value for Feature PressAndRelease EOSMethod. |
| 205 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.PressAndRelease.EOSTimeout` | readWrite | int | RFC configuration value for Feature PressAndRelease EOSTimeout. |
| 206 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ProtectX1Services.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ProtectX1Services Enable. |
| 207 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKBROWSER2_CFG.BACKGROUND_MEM_USAGE_HIGH_WATERMARK` | readWrite | unsignedInt | RFC configuration value for Feature RDKBROWSER2 CFG BACKGROUND MEM USAGE HIGH WATERMARK. |
| 208 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKBROWSER2_CFG.BACKGROUND_MEM_USAGE_LOW_WATERMARK` | readWrite | unsignedInt | RFC configuration value for Feature RDKBROWSER2 CFG BACKGROUND MEM USAGE LOW WATERMARK. |
| 209 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKBROWSER2_CFG.HTML_APP_RESURRECTION_PERIOD.days` | readWrite | unsignedInt | RFC configuration value for Feature RDKBROWSER2 CFG HTML APP RESURRECTION PERIOD days. |
| 210 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable` | readWrite | boolean | RFC flag that enables or disables Feature RDKRemoteDebugger Enable. |
| 211 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType` | readWrite | string | RFC configuration value for Feature RDKRemoteDebugger IssueType. |
| 212 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.WebCfgData` | readWrite | string | RFC configuration value for Feature RDKRemoteDebugger WebCfgData. |
| 213 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDMDecoupledVersionManagement.Enable` | readWrite | boolean | RFC flag that enables or disables Feature RDMDecoupledVersionManagement Enable. |
| 214 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.AudioProfileTarget` | readWrite | int | RFC configuration value for Feature RF4CE AudioProfileTarget. |
| 215 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.FF.RspIdle` | readWrite | int | RFC configuration value for Feature RF4CE FF RspIdle. |
| 216 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.HostPacketDecryption.Enable` | readWrite | boolean | RFC flag that enables or disables Feature RF4CE HostPacketDecryption Enable. |
| 217 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.OpusEncoderParams` | readWrite | string | RFC configuration value for Feature RF4CE OpusEncoderParams. |
| 218 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.RspTime.XDIU` | readWrite | int | RFC configuration value for Feature RF4CE RspTime XDIU. |
| 219 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.RspTime.XRC` | readWrite | int | RFC configuration value for Feature RF4CE RspTime XRC. |
| 220 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.RspTime.XVP` | readWrite | int | RFC configuration value for Feature RF4CE RspTime XVP. |
| 221 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RF4CE.VoiceEncryption.Enable` | readWrite | boolean | RFC flag that enables or disables Feature RF4CE VoiceEncryption Enable. |
| 222 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Detection` | readWrite | boolean | RFC configuration value for Feature RebootStop Detection. |
| 223 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Duration` | readWrite | int | RFC configuration value for Feature RebootStop Duration. |
| 224 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RebootStop.Enable` | readWrite | boolean | RFC flag that enables or disables Feature RebootStop Enable. |
| 225 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RedRecovery.Status` | readWrite | string | Current value reported for RFC setting Feature RedRecovery Status. |
| 226 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Resourcemanager.Blacklist.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Resourcemanager Blacklist Enable. |
| 227 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Resourcemanager.ReserveTTS.Enable` | readWrite | boolean | RFC flag that enables or disables Feature Resourcemanager ReserveTTS Enable. |
| 228 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RoamTrigger` | readWrite | string | RFC configuration value for Feature RoamTrigger. |
| 229 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SDCARD_SCRATCHPAD.Enable` | readWrite | string | RFC flag that enables or disables Feature SDCARD SCRATCHPAD Enable. |
| 230 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SHORTS.Enable` | readWrite | boolean | RFC flag that enables or disables Feature SHORTS Enable. |
| 231 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.STAGE.Enable` | readWrite | boolean | RFC flag that enables or disables Feature STAGE Enable. |
| 232 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLDirect.Enable` | readWrite | boolean | RFC flag that enables or disables Feature SWDLDirect Enable. |
| 233 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable` | readWrite | boolean | RFC flag that enables or disables Feature SWDLSpLimit Enable. |
| 234 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.LowSpeed` | readWrite | int | RFC configuration value for Feature SWDLSpLimit LowSpeed. |
| 235 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed` | readWrite | int | RFC configuration value for Feature SWDLSpLimit TopSpeed. |
| 236 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ScreenCapture.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ScreenCapture Enable. |
| 237 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ScreenCapture.URL` | readWrite | string | Endpoint URL used by RFC setting Feature ScreenCapture URL. |
| 238 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SecDump.Enable` | readWrite | boolean | RFC flag that enables or disables Feature SecDump Enable. |
| 239 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SoundPlayer.Enable` | readWrite | boolean | RFC flag that enables or disables Feature SoundPlayer Enable. |
| 240 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StandbyReboot.Enable` | readWrite | boolean | RFC flag that enables or disables Feature StandbyReboot Enable. |
| 241 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StandbyReboot.ForceAutoReboot` | readWrite | int | RFC configuration value for Feature StandbyReboot ForceAutoReboot. |
| 242 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StandbyReboot.StandbyAutoReboot` | readWrite | int | RFC configuration value for Feature StandbyReboot StandbyAutoReboot. |
| 243 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StopMaintenance.Enable` | readWrite | boolean | RFC flag that enables or disables StopMaintenance. |
| 244 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StorageManager.maxAdaptiveBitRate` | readWrite | int | RFC configuration value for Feature StorageManager maxAdaptiveBitRate. |
| 245 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StorageManager.maxTSBDurationMinutes` | readWrite | int | RFC configuration value for Feature StorageManager maxTSBDurationMinutes. |
| 246 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SupportedLanguages` | readWrite | string | RFC configuration value for Feature SupportedLanguages. |
| 247 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.TR069support.Enable` | readWrite | boolean | RFC flag that enables or disables Feature TR069support Enable. |
| 248 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.ConfigURL` | readWrite | string | Endpoint URL used by RFC setting Feature Telemetry ConfigURL. |
| 249 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.FTUEReport.Enable` | readWrite | boolean | RFC flag that enables or disables Telemetry FTUEReport. |
| 250 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.Version` | readWrite | string | Version value for RFC setting Feature Telemetry Version. |
| 251 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.TelemetryEndpoint.RedTmURL` | readWrite | string | RFC configuration value for Feature TelemetryEndpoint RedTmURL. |
| 252 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ThreadMonitorMinidump.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ThreadMonitorMinidump Enable. |
| 253 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ThreadPrioConfig.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ThreadPrioConfig Enable. |
| 254 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ThunderSecurity.Enable` | readWrite | boolean | RFC flag that enables or disables Feature ThunderSecurity Enable. |
| 255 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.TvSettings.DynamicAutoLatency` | readWrite | boolean | RFC configuration value for Feature TvSettings DynamicAutoLatency. |
| 256 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UNII3.Enable` | readWrite | boolean | RFC flag that enables or disables Feature UNII3 Enable. |
| 257 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UPnP.Refactor.Enable` | readWrite | boolean | RFC flag that enables or disables Feature UPnP Refactor Enable. |
| 258 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UPnPxPKI.Enable` | readWrite | boolean | RFC flag that enables or disables Feature UPnPxPKI Enable. |
| 259 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.USB_AutoMount.Enable` | readWrite | string | RFC flag that enables or disables Feature USB AutoMount Enable. |
| 260 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.USB_HID.Enable` | readWrite | boolean | RFC flag that enables or disables Feature USB HID Enable. |
| 261 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UhdEnable.Enable` | writeOnly | boolean | RFC flag that enables or disables UhdEnable. |
| 262 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UploadLogsOnUnscheduledReboot.Disable` | readWrite | boolean | RFC configuration value for Feature UploadLogsOnUnscheduledReboot Disable. |
| 263 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.VideoTelemetry.FrequncyMinutes` | readWrite | unsignedInt | RFC configuration value for Feature VideoTelemetry FrequncyMinutes. |
| 264 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ViperPPUrl` | readWrite | string | Endpoint URL used by RFC setting Feature ViperPPUrl. |
| 265 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.AudioConfidenceThreshold` | readWrite | string | RFC configuration value for Feature Voice AudioConfidenceThreshold. |
| 266 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.AudioDuckingLevel` | readWrite | string | RFC configuration value for Feature Voice AudioDuckingLevel. |
| 267 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.AudioDuckingType` | readWrite | int | RFC configuration value for Feature Voice AudioDuckingType. |
| 268 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.AudioMode` | readWrite | int | RFC configuration value for Feature Voice AudioMode. |
| 269 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.AudioTiming` | readWrite | int | RFC configuration value for Feature Voice AudioTiming. |
| 270 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.KeywordSensitivity` | readWrite | string | RFC configuration value for Feature Voice KeywordSensitivity. |
| 271 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Voice.VSDKConfiguration` | readWrite | string | RFC configuration value for Feature Voice VSDKConfiguration. |
| 272 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFi.WiFiStatsLogInterval` | readWrite | unsignedInt | RFC configuration value for Feature WiFi WiFiStatsLogInterval. |
| 273 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.Enable` | readWrite | boolean | RFC flag that enables or disables Feature WiFiReset Enable. |
| 274 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.EthernetLoggingInterval` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset EthernetLoggingInterval. |
| 275 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.PacketLossLoggingInterval` | readWrite | string | RFC configuration value for Feature WiFiReset PacketLossLoggingInterval. |
| 276 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.ReassociateTolerance` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset ReassociateTolerance. |
| 277 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.WifiLoggingInterval` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset WifiLoggingInterval. |
| 278 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.WifiReassociateInterval` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset WifiReassociateInterval. |
| 279 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.WifiResetIntervalForDriverIssue` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset WifiResetIntervalForDriverIssue. |
| 280 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.WifiResetIntervalForPacketLoss` | readWrite | unsignedInt | RFC configuration value for Feature WiFiReset WifiResetIntervalForPacketLoss. |
| 281 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WifiOptimizer.Enable` | readWrite | boolean | RFC flag that enables or disables Feature WifiOptimizer Enable. |
| 282 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XDial.AppList` | readWrite | string | RFC configuration value for Feature XDial AppList. |
| 283 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRDsp.XR19.Configuration` | readWrite | string | RFC configuration value for Feature XRDsp XR19 Configuration. |
| 284 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPairing.ASBDerivationMethod` | readWrite | int | RFC configuration value for Feature XRPairing ASBDerivationMethod. |
| 285 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPairing.ASBEnable` | readWrite | boolean | RFC configuration value for Feature XRPairing ASBEnable. |
| 286 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPairing.ASBFailThreshold` | readWrite | int | RFC configuration value for Feature XRPairing ASBFailThreshold. |
| 287 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.Default` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration Default. |
| 288 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.Enable` | readWrite | boolean | RFC flag that enables or disables Feature XRPollingConfiguration Enable. |
| 289 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XR11v2` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XR11v2. |
| 290 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XR15v1` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XR15v1. |
| 291 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XR15v2` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XR15v2. |
| 292 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XR16v1` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XR16v1. |
| 293 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XR19v1` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XR19v1. |
| 294 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRPollingConfiguration.XRAv1` | readWrite | string | RFC configuration value for Feature XRPollingConfiguration XRAv1. |
| 295 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRmacPolling.Enable` | readWrite | boolean | RFC flag that enables or disables Feature XRmacPolling Enable. |
| 296 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XRmacPolling.macPollingInterval` | readWrite | int | RFC configuration value for Feature XRmacPolling macPollingInterval. |
| 297 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.collectd.Enable` | readWrite | boolean | RFC flag that enables or disables Feature collectd Enable. |
| 298 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.collectd.GraphiteURL` | readWrite | string | Endpoint URL used by RFC setting Feature collectd GraphiteURL. |
| 299 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.collectd.Hostname` | readWrite | string | RFC configuration value for Feature collectd Hostname. |
| 300 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.collectd.PortNumber` | readWrite | unsignedInt | RFC configuration value for Feature collectd PortNumber. |
| 301 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.device_update` | readWrite | string | RFC configuration value for Feature ctrlm device update. |
| 302 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.global` | readWrite | string | RFC configuration value for Feature ctrlm global. |
| 303 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.network_ble` | readWrite | string | RFC configuration value for Feature ctrlm network ble. |
| 304 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.network_ip` | readWrite | string | RFC configuration value for Feature ctrlm network ip. |
| 305 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.network_rf4ce` | readWrite | string | RFC configuration value for Feature ctrlm network rf4ce. |
| 306 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.telemetry_report.ble` | readWrite | string | RFC configuration value for Feature ctrlm telemetry report ble. |
| 307 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.telemetry_report.global` | readWrite | string | RFC configuration value for Feature ctrlm telemetry report global. |
| 308 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.telemetry_report.ip` | readWrite | string | RFC configuration value for Feature ctrlm telemetry report ip. |
| 309 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.telemetry_report.rf4ce` | readWrite | string | RFC configuration value for Feature ctrlm telemetry report rf4ce. |
| 310 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.telemetry_report.voice` | readWrite | string | RFC configuration value for Feature ctrlm telemetry report voice. |
| 311 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.voice` | readWrite | string | RFC configuration value for Feature ctrlm voice. |
| 312 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ctrlm.vsdk` | readWrite | string | RFC configuration value for Feature ctrlm vsdk. |
| 313 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.eMMCFirmware.Version` | readWrite | string | Version value for RFC setting Feature eMMCFirmware Version. |
| 314 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.eMMCMitigation.Disable` | readWrite | boolean | RFC configuration value for Feature eMMCMitigation Disable. |
| 315 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.fkpskdf.Enable` | readWrite | boolean | RFC flag that enables or disables Feature fkpskdf Enable. |
| 316 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.Enable` | writeOnly | boolean | RFC flag that enables or disables hwHealthTest. |
| 317 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.EnablePeriodicRun` | writeOnly | boolean | RFC write-only configuration value for Feature hwHealthTest EnablePeriodicRun. |
| 318 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.PeriodicRunFrequency` | writeOnly | unsignedInt | RFC write-only configuration value for Feature hwHealthTest PeriodicRunFrequency. |
| 319 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.ResultFilter.Enable` | writeOnly | boolean | RFC flag that enables or disables hwHealthTest ResultFilter. |
| 320 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.ResultFilter.FilterParams` | writeOnly | string | RFC write-only configuration value for Feature hwHealthTest ResultFilter FilterParams. |
| 321 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.ResultFilter.QueueDepth` | writeOnly | unsignedInt | RFC write-only configuration value for Feature hwHealthTest ResultFilter QueueDepth. |
| 322 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.ResultFilter.ResultsFiltered` | writeOnly | boolean | RFC write-only configuration value for Feature hwHealthTest ResultFilter ResultsFiltered. |
| 323 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.cpuThreshold` | writeOnly | unsignedInt | RFC write-only configuration value for Feature hwHealthTest cpuThreshold. |
| 324 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.dramThreshold` | writeOnly | unsignedInt | RFC write-only configuration value for Feature hwHealthTest dramThreshold. |
| 325 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.memcapture.Duration` | readWrite | int | RFC configuration value for Feature memcapture Duration. |
| 326 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.memcapture.Enable` | readWrite | boolean | RFC flag that enables or disables Feature memcapture Enable. |
| 327 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Args` | readWrite | string | RFC configuration value for Feature meminsight Args. |
| 328 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Enable` | readWrite | boolean | RFC flag that enables or disables meminsight. |
| 329 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Trigger` | readWrite | string | RFC configuration value for Feature meminsight Trigger. |
| 330 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.processmonitor.Enable` | readWrite | boolean | RFC flag that enables or disables processmonitor. |
| 331 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.systemd-analyze.Enable` | readWrite | boolean | RFC flag that enables or disables Feature systemd analyze Enable. |
| 332 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.HostIf.ParodusTokenServerUrl` | readWrite | string | Endpoint URL used by RFC setting HostIf ParodusTokenServerUrl. |
| 333 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Identity.DbgServices.Enable` | readWrite | boolean | RFC flag that enables or disables Identity DbgServices Enable. |
| 334 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Identity.DeviceType` | readWrite | string | RFC configuration value for Identity DeviceType. |
| 335 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcaUploadPRODUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcaUploadPRODUrl. |
| 336 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcaUploadUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcaUploadUrl. |
| 337 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcmLogServerCQAUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcmLogServerCQAUrl. |
| 338 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcmLogServerDEVUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcmLogServerDEVUrl. |
| 339 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcmLogServerPRODUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcmLogServerPRODUrl. |
| 340 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcmLogUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcmLogUrl. |
| 341 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.DcmScpServerUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload DcmScpServerUrl. |
| 342 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.LogServerUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload LogServerUrl. |
| 343 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.S3SignedBucketUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload S3SignedBucketUrl. |
| 344 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.LogUpload.SsrUrl` | readWrite | string | Endpoint URL used by RFC setting LogUpload SsrUrl. |
| 345 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SecManager.dieselUrl` | readWrite | string | Endpoint URL used by RFC setting SecManager dieselUrl. |
| 346 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.Activation` | readWrite | string | RFC configuration value for SocProvisioning Activation. |
| 347 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.AuthMessage` | readWrite | string | RFC configuration value for SocProvisioning AuthMessage. |
| 348 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.NameSpacePrefix` | readWrite | string | RFC configuration value for SocProvisioning NameSpacePrefix. |
| 349 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.NameSpaceUri` | readWrite | string | RFC configuration value for SocProvisioning NameSpaceUri. |
| 350 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.Renewal` | readWrite | string | RFC configuration value for SocProvisioning Renewal. |
| 351 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.URL1` | readWrite | string | RFC configuration value for SocProvisioning URL1. |
| 352 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.URL2` | readWrite | string | RFC configuration value for SocProvisioning URL2. |
| 353 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.backoffIntervalMax` | readWrite | unsignedLong | RFC configuration value for SocProvisioning backoffIntervalMax. |
| 354 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SocProvisioning.disableCredentialsPrefetchCaching` | readWrite | boolean | RFC configuration value for SocProvisioning disableCredentialsPrefetchCaching. |
| 355 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.CCRProxyUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint CCRProxyUrl. |
| 356 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.CDNCCPUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint CDNCCPUrl. |
| 357 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.CcpUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint CcpUrl. |
| 358 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.DAC15CDLUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint DAC15CDLUrl. |
| 359 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.EDGEDNSProxyUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint EDGEDNSProxyUrl. |
| 360 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.FkpsBrokerUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint FkpsBrokerUrl. |
| 361 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.FkpsUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint FkpsUrl. |
| 362 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.PRODCDLUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint PRODCDLUrl. |
| 363 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.XcalUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint XcalUrl. |
| 364 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.XconfDEVUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint XconfDEVUrl. |
| 365 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Sysint.XconfUrl` | readWrite | string | Endpoint URL used by RFC setting Sysint XconfUrl. |
| 366 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.FriendlyName` | readWrite | string | RFC configuration value for SystemServices FriendlyName. |
| 367 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TextToSpeech.URL` | readWrite | string | Endpoint URL used by RFC setting TextToSpeech URL. |
| 368 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.AspectRatio` | readWrite | string | TV picture setting for AspectRatio. |
| 369 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.AutoBacklightMode` | readWrite | string | TV picture setting for AutoBacklightMode. |
| 370 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.Backlight` | readWrite | int | TV picture setting for Backlight. |
| 371 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.Boost` | readWrite | int | TV picture setting for Boost. |
| 372 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.Brightness` | readWrite | int | TV picture setting for Brightness. |
| 373 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.ColorTemperature` | readWrite | string | TV picture setting for ColorTemperature. |
| 374 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.Contrast` | readWrite | int | TV picture setting for Contrast. |
| 375 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.DolbyVisionMode` | readWrite | string | TV picture setting for DolbyVisionMode. |
| 376 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.DynamicContrastModeString` | readWrite | string | TV picture setting for DynamicContrastModeString. |
| 377 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.Hue` | readWrite | int | TV picture setting for Hue. |
| 378 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.PictureMode` | readWrite | int | TV picture setting for PictureMode. |
| 379 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.PictureModeString` | readWrite | string | TV picture setting for PictureModeString. |
| 380 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.Saturation` | readWrite | int | TV picture setting for Saturation. |
| 381 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.TvSettings.Sharpness` | readWrite | int | TV picture setting for Sharpness. |
| 382 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.LRHAcceptValue` | readWrite | string | RFC configuration value for aamp LRHAcceptValue. |
| 383 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.LRHContentType` | readWrite | string | RFC configuration value for aamp LRHContentType. |
| 384 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.SchemeIdUriDaiStream` | readWrite | string | RFC configuration value for aamp SchemeIdUriDaiStream. |
| 385 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.SchemeIdUriVssStream` | readWrite | string | RFC configuration value for aamp SchemeIdUriVssStream. |
| 386 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.SlowMotion` | readWrite | boolean | RFC configuration value for aamp SlowMotion. |
| 387 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.asyncTune` | readWrite | boolean | RFC configuration value for aamp asyncTune. |
| 388 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.cdvrLiveOffset` | readWrite | unsignedLong | RFC configuration value for aamp cdvrLiveOffset. |
| 389 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.client-dai` | readWrite | boolean | RFC configuration value for aamp client dai. |
| 390 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.curlStore` | readWrite | boolean | RFC configuration value for aamp curlStore. |
| 391 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.defaultBitrate` | readWrite | unsignedLong | RFC configuration value for aamp defaultBitrate. |
| 392 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.defaultBitrate4K` | readWrite | unsignedLong | RFC configuration value for aamp defaultBitrate4K. |
| 393 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.disableAC3` | readWrite | boolean | RFC configuration value for aamp disableAC3. |
| 394 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.disableAC4` | readWrite | boolean | RFC configuration value for aamp disableAC4. |
| 395 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.disableATMOS` | readWrite | boolean | RFC configuration value for aamp disableATMOS. |
| 396 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.disableEC3` | readWrite | boolean | RFC configuration value for aamp disableEC3. |
| 397 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enableCMCD` | readWrite | boolean | RFC configuration value for aamp enableCMCD. |
| 398 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enableLiveLatencyCorrection` | readWrite | boolean | RFC configuration value for aamp enableLiveLatencyCorrection. |
| 399 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enableLowLatencyCorrection` | readWrite | boolean | RFC configuration value for aamp enableLowLatencyCorrection. |
| 400 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enableLowLatencyDash` | readWrite | boolean | RFC configuration value for aamp enableLowLatencyDash. |
| 401 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enablePTSReStamp` | readWrite | boolean | RFC configuration value for aamp enablePTSReStamp. |
| 402 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.enableVideoEndEvent` | readWrite | boolean | RFC configuration value for aamp enableVideoEndEvent. |
| 403 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.info` | readWrite | boolean | RFC configuration value for aamp info. |
| 404 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.liveOffset` | readWrite | unsignedLong | RFC configuration value for aamp liveOffset. |
| 405 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.liveOffset4K` | readWrite | string | RFC configuration value for aamp liveOffset4K. |
| 406 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.manifestTimeout` | readWrite | unsignedLong | RFC configuration value for aamp manifestTimeout. |
| 407 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.maxBitrate` | readWrite | unsignedLong | RFC configuration value for aamp maxBitrate. |
| 408 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.maxInitFragCachePerTrack` | readWrite | int | RFC configuration value for aamp maxInitFragCachePerTrack. |
| 409 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.minBitrate` | readWrite | unsignedLong | RFC configuration value for aamp minBitrate. |
| 410 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.networkTimeout` | readWrite | unsignedLong | RFC configuration value for aamp networkTimeout. |
| 411 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.persistBitrateOverSeek` | readWrite | boolean | RFC configuration value for aamp persistBitrateOverSeek. |
| 412 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.playlistTimeout` | readWrite | unsignedLong | RFC configuration value for aamp playlistTimeout. |
| 413 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.preferredDrm` | readWrite | int | RFC configuration value for aamp preferredDrm. |
| 414 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.rateCorrectionDelay` | readWrite | unsignedLong | RFC configuration value for aamp rateCorrectionDelay. |
| 415 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.sharedSSL` | readWrite | boolean | RFC configuration value for aamp sharedSSL. |
| 416 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.stereoOnly` | readWrite | boolean | RFC configuration value for aamp stereoOnly. |
| 417 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.supportTLS` | readWrite | unsignedLong | RFC configuration value for aamp supportTLS. |
| 418 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.tsbInterruptHandling` | readWrite | boolean | RFC configuration value for aamp tsbInterruptHandling. |
| 419 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.useSecManager` | readWrite | boolean | RFC configuration value for aamp useSecManager. |
| 420 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.aamp.useWesterosSink` | readWrite | boolean | RFC configuration value for aamp useWesterosSink. |
| 421 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.apps.watchdogmode` | readWrite | boolean | RFC configuration value for apps watchdogmode. |
| 422 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.fog.ZeroDrmHost` | readWrite | string | RFC configuration value for fog ZeroDrmHost. |
| 423 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.recorder.LongpollUrl` | readWrite | string | Endpoint URL used by RFC setting recorder LongpollUrl. |
| 424 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.recorder.Status` | readWrite | string | Current value reported for RFC setting recorder Status. |
| 425 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.recorder.UpdateSchedule` | readWrite | string | RFC configuration value for recorder UpdateSchedule. |
| 426 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.vodclient.StunnelConnect` | readWrite | string | RFC configuration value for vodclient StunnelConnect. |
| 427 | `Device.DeviceInfo.X_RDKCENTRAL-COM_Reset` | readWrite | string | Triggers the platform-specific reset workflow. |
| 428 | `Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId` | readWrite | string | Partner identifier used for syndication. |
| 429 | `Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action` | readWrite | string | XR polling configuration value for Action. |
| 430 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.LastResetTime` | readOnly | unsignedLong | Timestamp of the most recent reset event. |
| 431 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xAccount.HotelCheckout.Status` | readOnly | string | Current status of the device. |
| 432 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.BLE.Tile.Cmd.Request` | readWrite | string | Bluetooth subsystem value for Request. |
| 433 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.Active` | readOnly | int | Indicates whether the related entry is currently active. |
| 434 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.DeviceID` | readOnly | string | Device identifier reported for the related entry. |
| 435 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.DeviceType` | readOnly | string | Device type reported for the related entry. |
| 436 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.Name` | readOnly | string | Name reported for this connected Bluetooth device. |
| 437 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDeviceCnt` | readOnly | unsignedInt | Number of currently connected Bluetooth device entries. |
| 438 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.DeviceID` | readOnly | string | Device identifier reported for the related entry. |
| 439 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.MAC` | readOnly | string | MAC address associated with the local Bluetooth adapter. |
| 440 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.Manufacturer` | readOnly | int | Manufacturer reported for the local Bluetooth adapter. |
| 441 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.Profile` | readOnly | string | Profile reported for the related entry. |
| 442 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.RSSI` | readOnly | int | RSSI reported for the related entry. |
| 443 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.SignalStrength` | readOnly | string | Signal strength reported for the related entry. |
| 444 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.DeviceID` | readOnly | string | Device identifier reported for the related entry. |
| 445 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.DeviceType` | readOnly | string | Device type reported for the related entry. |
| 446 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.Name` | readOnly | string | Name reported for this discovered Bluetooth device. |
| 447 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.Paired` | readOnly | boolean | Indicates whether the related Bluetooth device is paired. |
| 448 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDeviceCnt` | readOnly | unsignedInt | Number of discovered Bluetooth device entries currently reported. |
| 449 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveryEnabled` | readOnly | boolean | Reports whether Bluetooth discovery is currently enabled. |
| 450 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.Enabled` | readOnly | string | Current enabled state of the device. |
| 451 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.GetDeviceInfo` | readOnly | string | Snapshot of detailed Bluetooth adapter information. |
| 452 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.LimitBeaconDetection` | readWrite | boolean | Controls whether Bluetooth beacon detection is restricted by platform policy. |
| 453 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.Connected` | readOnly | boolean | Indicates whether the related device is currently connected. |
| 454 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.DeviceID` | readOnly | string | Device identifier reported for the related entry. |
| 455 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.DeviceType` | readOnly | string | Device type reported for the related entry. |
| 456 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.Name` | readOnly | string | Name reported for this paired Bluetooth device. |
| 457 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDeviceCnt` | readOnly | unsignedInt | Number of paired Bluetooth device entries currently reported. |
| 458 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable` | readWrite | boolean | Enables or disables the xOps forward SSH access path. |
| 459 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMLogsUploadStatus` | readOnly | string | Current xOps device management log upload status. |
| 460 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow` | writeOnly | boolean | xOps device management write-only trigger for xOpsDMUploadLogsNow. |
| 461 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.DeviceManageableNotification` | readWrite | string | Manageable-device notification payload for xOps RPC. |
| 462 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadCompletedNotification` | readWrite | boolean | xOps RPC notification control for the named event. |
| 463 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.FirmwareDownloadStartedNotification` | readWrite | string | xOps RPC notification control for the named event. |
| 464 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow` | writeOnly | boolean | xOps device management write-only trigger for RebootNow. |
| 465 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootPendingNotification` | readWrite | unsignedInt | xOps RPC notification control for the named event. |
| 466 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs` | readWrite | string | Arguments used to start reverse SSH through xOps. |
| 467 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshStatus` | readOnly | string | Current reverse SSH status reported by xOps. |
| 468 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger` | writeOnly | string | xOps device management write-only trigger for xOpsReverseSshTrigger. |
| 469 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.ExecuteTest` | writeOnly | int | xOps device management write-only trigger for ExecuteTest. |
| 470 | `Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.Results` | readOnly | string | Result payload reported by the related diagnostic or test. |
| 471 | `Device.DeviceInfo.X_RDK_FirmwareName` | readOnly | string | Firmware image name currently reported by the RDK platform. |
| 472 | `Device.DeviceInfo.X_RDK_RDKProfileName` | readWrite | string | Active RDK profile name associated with the device configuration. |
| 473 | `Device.Ethernet.Interface.{i}.Alias` | readWrite | string | User-assigned alias for this Ethernet interface. |
| 474 | `Device.Ethernet.Interface.{i}.DuplexMode` | readWrite | string | Configured or reported duplex mode for the interface. |
| 475 | `Device.Ethernet.Interface.{i}.Enable` | readWrite | boolean | Enables or disables this Ethernet interface. |
| 476 | `Device.Ethernet.Interface.{i}.LastChange` | readOnly | unsignedInt | Seconds since this Ethernet interface last changed state. |
| 477 | `Device.Ethernet.Interface.{i}.LowerLayers` | readWrite | string | Lower-layer interface references for this Ethernet interface. |
| 478 | `Device.Ethernet.Interface.{i}.MACAddress` | readOnly | string | MAC address associated with this Ethernet interface. |
| 479 | `Device.Ethernet.Interface.{i}.MaxBitRate` | readWrite | int | Configured or negotiated maximum link bit rate for this Ethernet interface. |
| 480 | `Device.Ethernet.Interface.{i}.Name` | readOnly | string | Name reported for this Ethernet interface. |
| 481 | `Device.Ethernet.Interface.{i}.Stats.BroadcastPacketsReceived` | readOnly | unsignedLong | Broadcast packets received on this Ethernet interface. |
| 482 | `Device.Ethernet.Interface.{i}.Stats.BroadcastPacketsSent` | readOnly | unsignedLong | Broadcast packets sent on this Ethernet interface. |
| 483 | `Device.Ethernet.Interface.{i}.Stats.BytesReceived` | readOnly | unsignedLong | Total bytes received on this Ethernet interface. |
| 484 | `Device.Ethernet.Interface.{i}.Stats.BytesSent` | readOnly | unsignedLong | Total bytes sent on this Ethernet interface. |
| 485 | `Device.Ethernet.Interface.{i}.Stats.DiscardPacketsReceived` | readOnly | unsignedInt | Received packets discarded on this Ethernet interface. |
| 486 | `Device.Ethernet.Interface.{i}.Stats.DiscardPacketsSent` | readOnly | unsignedInt | Outbound packets discarded on this Ethernet interface. |
| 487 | `Device.Ethernet.Interface.{i}.Stats.ErrorsReceived` | readOnly | unsignedInt | Receive errors seen on this Ethernet interface. |
| 488 | `Device.Ethernet.Interface.{i}.Stats.ErrorsSent` | readOnly | unsignedInt | Transmit errors seen on this Ethernet interface. |
| 489 | `Device.Ethernet.Interface.{i}.Stats.MulticastPacketsReceived` | readOnly | unsignedLong | Multicast packets received on this Ethernet interface. |
| 490 | `Device.Ethernet.Interface.{i}.Stats.MulticastPacketsSent` | readOnly | unsignedLong | Multicast packets sent on this Ethernet interface. |
| 491 | `Device.Ethernet.Interface.{i}.Stats.PacketsReceived` | readOnly | unsignedLong | Total packets received on this Ethernet interface. |
| 492 | `Device.Ethernet.Interface.{i}.Stats.PacketsSent` | readOnly | unsignedLong | Total packets sent on this Ethernet interface. |
| 493 | `Device.Ethernet.Interface.{i}.Stats.UnicastPacketsReceived` | readOnly | unsignedLong | Unicast packets received on this Ethernet interface. |
| 494 | `Device.Ethernet.Interface.{i}.Stats.UnicastPacketsSent` | readOnly | unsignedLong | Unicast packets sent on this Ethernet interface. |
| 495 | `Device.Ethernet.Interface.{i}.Stats.UnknownProtoPacketsReceived` | readOnly | unsignedInt | Packets with unknown protocol received on this Ethernet interface. |
| 496 | `Device.Ethernet.Interface.{i}.Status` | readOnly | string | Current status of this Ethernet interface. |
| 497 | `Device.Ethernet.Interface.{i}.Upstream` | readOnly | boolean | Indicates whether the interface is designated as upstream. |
| 498 | `Device.Ethernet.InterfaceNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 499 | `Device.Ethernet.Link.{i}.Enable` | readWrite | boolean | Enables or disables this Ethernet link. |
| 500 | `Device.Ethernet.Link.{i}.LowerLayers` | readWrite | string | Lower-layer interface references for this Ethernet link. |
| 501 | `Device.Ethernet.Link.{i}.MACAddress` | readOnly | string | MAC address associated with this Ethernet link. |
| 502 | `Device.Ethernet.Link.{i}.Name` | readOnly | string | Name reported for this Ethernet link. |
| 503 | `Device.Ethernet.Link.{i}.Status` | readOnly | string | Current status of this Ethernet link. |
| 504 | `Device.Ethernet.LinkNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 505 | `Device.IP.ActivePort.{i}.LocalIPAddress` | readOnly | string | IP address used by this active port entry. |
| 506 | `Device.IP.ActivePort.{i}.LocalPort` | readOnly | unsignedInt | Port value used by this active port entry. |
| 507 | `Device.IP.ActivePort.{i}.RemoteIPAddress` | readOnly | string | IP address used by this active port entry. |
| 508 | `Device.IP.ActivePort.{i}.RemotePort` | readOnly | unsignedInt | Port value used by this active port entry. |
| 509 | `Device.IP.ActivePort.{i}.Status` | readOnly | string | Current status of this active port entry. |
| 510 | `Device.IP.ActivePortNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 511 | `Device.IP.Diagnostics.DownloadDiagnostics.BOMTime` | readOnly | dateTime | Beginning of measurement time for the diagnostic run. |
| 512 | `Device.IP.Diagnostics.DownloadDiagnostics.DSCP` | readWrite | unsignedInt | DSCP value used by the diagnostic traffic. |
| 513 | `Device.IP.Diagnostics.DownloadDiagnostics.DiagnosticsState` | readWrite | string | Execution state of the diagnostic. |
| 514 | `Device.IP.Diagnostics.DownloadDiagnostics.DownloadTransports` | readOnly | string | Transfer transports supported by the diagnostic. |
| 515 | `Device.IP.Diagnostics.DownloadDiagnostics.DownloadURL` | readWrite | string | Target URL used by the diagnostic. |
| 516 | `Device.IP.Diagnostics.DownloadDiagnostics.EOMTime` | readOnly | dateTime | End of measurement time for the diagnostic run. |
| 517 | `Device.IP.Diagnostics.DownloadDiagnostics.EthernetPriority` | readWrite | unsignedInt | Ethernet priority used by the diagnostic traffic. |
| 518 | `Device.IP.Diagnostics.DownloadDiagnostics.Interface` | readWrite | string | Interface reference used by the download diagnostic. |
| 519 | `Device.IP.Diagnostics.DownloadDiagnostics.ROMTime` | readOnly | dateTime | Request start time for the diagnostic run. |
| 520 | `Device.IP.Diagnostics.DownloadDiagnostics.TCPOpenRequestTime` | readOnly | dateTime | Timestamp when the diagnostic opened the TCP connection request. |
| 521 | `Device.IP.Diagnostics.DownloadDiagnostics.TCPOpenResponseTime` | readOnly | dateTime | Timestamp when the diagnostic received the TCP connection response. |
| 522 | `Device.IP.Diagnostics.DownloadDiagnostics.TestBytesReceived` | readOnly | unsignedInt | Configured or measured test payload size for the diagnostic. |
| 523 | `Device.IP.Diagnostics.DownloadDiagnostics.TotalBytesReceived` | readOnly | unsignedInt | Total payload bytes transferred during the diagnostic. |
| 524 | `Device.IP.Diagnostics.IPPing.AverageResponseTime` | readOnly | unsignedInt | Measured response time reported by the diagnostic. |
| 525 | `Device.IP.Diagnostics.IPPing.DSCP` | readWrite | unsignedInt | DSCP value used by the diagnostic traffic. |
| 526 | `Device.IP.Diagnostics.IPPing.DataBlockSize` | readWrite | unsignedInt | Payload size used by the diagnostic packets. |
| 527 | `Device.IP.Diagnostics.IPPing.DiagnosticsState` | readWrite | string | Execution state of the diagnostic. |
| 528 | `Device.IP.Diagnostics.IPPing.FailureCount` | readOnly | unsignedInt | Count of failed or successful attempts in the diagnostic run. |
| 529 | `Device.IP.Diagnostics.IPPing.Host` | readWrite | string | Host name or address used by the IP ping diagnostic. |
| 530 | `Device.IP.Diagnostics.IPPing.Interface` | readWrite | string | Interface reference used by the IP ping diagnostic. |
| 531 | `Device.IP.Diagnostics.IPPing.MaximumResponseTime` | readOnly | unsignedInt | Measured response time reported by the diagnostic. |
| 532 | `Device.IP.Diagnostics.IPPing.MinimumResponseTime` | readOnly | unsignedInt | Measured response time reported by the diagnostic. |
| 533 | `Device.IP.Diagnostics.IPPing.NumberOfRepetitions` | readWrite | unsignedInt | Number of attempts configured for the diagnostic. |
| 534 | `Device.IP.Diagnostics.IPPing.SuccessCount` | readOnly | unsignedInt | Count of failed or successful attempts in the diagnostic run. |
| 535 | `Device.IP.Diagnostics.IPPing.Timeout` | readWrite | unsignedInt | Timeout value used by the diagnostic run. |
| 536 | `Device.IP.Diagnostics.TraceRoute.DSCP` | readWrite | unsignedInt | DSCP value used by the diagnostic traffic. |
| 537 | `Device.IP.Diagnostics.TraceRoute.DataBlockSize` | readWrite | unsignedInt | Payload size used by the diagnostic packets. |
| 538 | `Device.IP.Diagnostics.TraceRoute.DiagnosticsState` | readWrite | string | Execution state of the diagnostic. |
| 539 | `Device.IP.Diagnostics.TraceRoute.Host` | readWrite | string | Host name or address used by the traceroute diagnostic. |
| 540 | `Device.IP.Diagnostics.TraceRoute.Interface` | readWrite | string | Interface reference used by the traceroute diagnostic. |
| 541 | `Device.IP.Diagnostics.TraceRoute.MaxHopCount` | readWrite | unsignedInt | Maximum hop count allowed for the traceroute run. |
| 542 | `Device.IP.Diagnostics.TraceRoute.NumberOfTries` | readWrite | unsignedInt | Number of attempts configured for the diagnostic. |
| 543 | `Device.IP.Diagnostics.TraceRoute.ResponseTime` | readOnly | unsignedInt | Measured response time reported by the diagnostic. |
| 544 | `Device.IP.Diagnostics.TraceRoute.RouteHops.{i}.ErrorCode` | readOnly | unsignedInt | Error code reported for this traceroute hop. |
| 545 | `Device.IP.Diagnostics.TraceRoute.RouteHops.{i}.Host` | readOnly | string | Host name or address used by this traceroute hop. |
| 546 | `Device.IP.Diagnostics.TraceRoute.RouteHops.{i}.HostAddress` | readOnly | string | Resolved host address for this traceroute hop. |
| 547 | `Device.IP.Diagnostics.TraceRoute.RouteHops.{i}.RTTimes` | readOnly | string | Round-trip time samples for this traceroute hop. |
| 548 | `Device.IP.Diagnostics.TraceRoute.RouteHopsNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the traceroute diagnostic. |
| 549 | `Device.IP.Diagnostics.TraceRoute.Timeout` | readWrite | unsignedInt | Timeout value used by the diagnostic run. |
| 550 | `Device.IP.Diagnostics.UDPEchoConfig.BytesReceived` | readOnly | unsignedInt | Total bytes received by the related diagnostic or service. |
| 551 | `Device.IP.Diagnostics.UDPEchoConfig.BytesResponded` | readOnly | unsignedInt | Total bytes sent in response by the related diagnostic or service. |
| 552 | `Device.IP.Diagnostics.UDPEchoConfig.EchoPlusEnabled` | readWrite | boolean | Echo Plus capability state for the UDP echo server. |
| 553 | `Device.IP.Diagnostics.UDPEchoConfig.EchoPlusSupported` | readOnly | boolean | Echo Plus capability state for the UDP echo server. |
| 554 | `Device.IP.Diagnostics.UDPEchoConfig.Enable` | readWrite | boolean | Enables or disables the UDP echo server. |
| 555 | `Device.IP.Diagnostics.UDPEchoConfig.Interface` | readWrite | string | Interface reference used by the UDP echo server. |
| 556 | `Device.IP.Diagnostics.UDPEchoConfig.PacketsReceived` | readOnly | unsignedInt | Packet count recorded by the UDP echo server. |
| 557 | `Device.IP.Diagnostics.UDPEchoConfig.PacketsResponded` | readOnly | unsignedInt | Packet count recorded by the UDP echo server. |
| 558 | `Device.IP.Diagnostics.UDPEchoConfig.SourceIPAddress` | readWrite | string | IP address used by the UDP echo server. |
| 559 | `Device.IP.Diagnostics.UDPEchoConfig.TimeFirstPacketReceived` | readOnly | dateTime | Timestamp of the first or last packet seen by the UDP echo server. |
| 560 | `Device.IP.Diagnostics.UDPEchoConfig.TimeLastPacketReceived` | readOnly | dateTime | Timestamp of the first or last packet seen by the UDP echo server. |
| 561 | `Device.IP.Diagnostics.UDPEchoConfig.UDPPort` | readWrite | unsignedInt | Port value used by the UDP echo server. |
| 562 | `Device.IP.Diagnostics.UploadDiagnostics.BOMTime` | readOnly | dateTime | Beginning of measurement time for the diagnostic run. |
| 563 | `Device.IP.Diagnostics.UploadDiagnostics.DSCP` | readWrite | unsignedInt | DSCP value used by the diagnostic traffic. |
| 564 | `Device.IP.Diagnostics.UploadDiagnostics.DiagnosticsState` | readWrite | string | Execution state of the diagnostic. |
| 565 | `Device.IP.Diagnostics.UploadDiagnostics.EOMTime` | readOnly | dateTime | End of measurement time for the diagnostic run. |
| 566 | `Device.IP.Diagnostics.UploadDiagnostics.EthernetPriority` | readWrite | unsignedInt | Ethernet priority used by the diagnostic traffic. |
| 567 | `Device.IP.Diagnostics.UploadDiagnostics.Interface` | readWrite | string | Interface reference used by the upload diagnostic. |
| 568 | `Device.IP.Diagnostics.UploadDiagnostics.ROMTime` | readOnly | dateTime | Request start time for the diagnostic run. |
| 569 | `Device.IP.Diagnostics.UploadDiagnostics.TCPOpenRequestTime` | readOnly | dateTime | Timestamp when the diagnostic opened the TCP connection request. |
| 570 | `Device.IP.Diagnostics.UploadDiagnostics.TCPOpenResponseTime` | readOnly | dateTime | Timestamp when the diagnostic received the TCP connection response. |
| 571 | `Device.IP.Diagnostics.UploadDiagnostics.TestFileLength` | readWrite | unsignedInt | Configured or measured test payload size for the diagnostic. |
| 572 | `Device.IP.Diagnostics.UploadDiagnostics.TotalBytesSent` | readOnly | unsignedInt | Total payload bytes transferred during the diagnostic. |
| 573 | `Device.IP.Diagnostics.UploadDiagnostics.UploadTransports` | readOnly | string | Transfer transports supported by the diagnostic. |
| 574 | `Device.IP.Diagnostics.UploadDiagnostics.UploadURL` | readWrite | string | Target URL used by the diagnostic. |
| 575 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Argument` | readWrite | string | Input value used by the RDK speed test. |
| 576 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Authentication` | readWrite | string | Input value used by the RDK speed test. |
| 577 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.ClientType` | readWrite | unsignedInt | Client type used by the RDK speed test. |
| 578 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Enable` | readWrite | boolean | Enables or disables the RDK speed test. |
| 579 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Enable_Speedtest` | readWrite | boolean | Configuration or status value for the RDK speed test. |
| 580 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Run` | readWrite | boolean | Triggers immediate execution of the related diagnostic or action. |
| 581 | `Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.Status` | readOnly | unsignedInt | Current status of the RDK speed test. |
| 582 | `Device.IP.IPv4Capable` | readOnly | boolean | Indicates whether the device supports IPv4. |
| 583 | `Device.IP.IPv4Enable` | readWrite | boolean | Enables or disables IPv4 on this object. |
| 584 | `Device.IP.IPv4Status` | readOnly | string | Current IPv4 operational status of the device. |
| 585 | `Device.IP.Interface.{i}.Alias` | readWrite | string | User-assigned alias for this IP interface. |
| 586 | `Device.IP.Interface.{i}.AutoIPEnable` | readWrite | boolean | Enables or disables AutoIP on this IP interface. |
| 587 | `Device.IP.Interface.{i}.Enable` | readWrite | boolean | Enables or disables this IP interface. |
| 588 | `Device.IP.Interface.{i}.IPv4Address.{i}.AddressingType` | readOnly | string | Addressing method used for this IPv4 address entry. |
| 589 | `Device.IP.Interface.{i}.IPv4Address.{i}.Alias` | readWrite | string | User-assigned alias for this IPv4 address entry. |
| 590 | `Device.IP.Interface.{i}.IPv4Address.{i}.Enable` | readWrite | boolean | Enables or disables this IPv4 address entry. |
| 591 | `Device.IP.Interface.{i}.IPv4Address.{i}.IPAddress` | readWrite | string | IP address associated with this IPv4 address entry. |
| 592 | `Device.IP.Interface.{i}.IPv4Address.{i}.Status` | readOnly | string | Current status of this IPv4 address entry. |
| 593 | `Device.IP.Interface.{i}.IPv4Address.{i}.SubnetMask` | readWrite | string | Subnet mask assigned to this IPv4 address entry. |
| 594 | `Device.IP.Interface.{i}.IPv4AddressNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this IP interface. |
| 595 | `Device.IP.Interface.{i}.IPv4Enable` | readWrite | boolean | Enables or disables IPv4 on this IP interface. |
| 596 | `Device.IP.Interface.{i}.IPv6Address.{i}.Alias` | readWrite | string | User-assigned alias for this IPv6 address entry. |
| 597 | `Device.IP.Interface.{i}.IPv6Address.{i}.Anycast` | readWrite | boolean | Indicates whether this IPv6 address is anycast. |
| 598 | `Device.IP.Interface.{i}.IPv6Address.{i}.Enable` | readWrite | boolean | Enables or disables this IPv6 address entry. |
| 599 | `Device.IP.Interface.{i}.IPv6Address.{i}.IPAddress` | readWrite | string | IP address associated with this IPv6 address entry. |
| 600 | `Device.IP.Interface.{i}.IPv6Address.{i}.IPAddressStatus` | readOnly | string | Current status of this IPv6 address. |
| 601 | `Device.IP.Interface.{i}.IPv6Address.{i}.Origin` | readOnly | string | Origin by which the related address or prefix was created. |
| 602 | `Device.IP.Interface.{i}.IPv6Address.{i}.PreferredLifetime` | readWrite | dateTime | Preferred lifetime for the related address or prefix. |
| 603 | `Device.IP.Interface.{i}.IPv6Address.{i}.Prefix` | readWrite | string | IP prefix associated with the related address or prefix entry. |
| 604 | `Device.IP.Interface.{i}.IPv6Address.{i}.Status` | readOnly | string | Current status of this IPv6 address entry. |
| 605 | `Device.IP.Interface.{i}.IPv6Address.{i}.ValidLifetime` | readWrite | dateTime | Valid lifetime for the related address or prefix. |
| 606 | `Device.IP.Interface.{i}.IPv6AddressNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this IP interface. |
| 607 | `Device.IP.Interface.{i}.IPv6Enable` | readWrite | boolean | Enables or disables IPv6 on this IP interface. |
| 608 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Alias` | readWrite | string | User-assigned alias for this IPv6 prefix entry. |
| 609 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Autonomous` | readWrite | boolean | Indicates whether the prefix is used for autonomous addressing. |
| 610 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.ChildPrefixBits` | readWrite | string | Child prefix bits delegated from this IPv6 prefix. |
| 611 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Enable` | readWrite | boolean | Enables or disables this IPv6 prefix entry. |
| 612 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.OnLink` | readWrite | boolean | Indicates whether the prefix is advertised as on-link. |
| 613 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Origin` | readOnly | string | Origin by which the related address or prefix was created. |
| 614 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.ParentPrefix` | readWrite | string | Parent prefix reference for this IPv6 prefix entry. |
| 615 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.PreferredLifetime` | readWrite | dateTime | Preferred lifetime for the related address or prefix. |
| 616 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Prefix` | readWrite | string | IP prefix associated with the related address or prefix entry. |
| 617 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.PrefixStatus` | readOnly | string | Current status of this IPv6 prefix. |
| 618 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.StaticType` | readWrite | string | Static type assigned to this IPv6 prefix entry. |
| 619 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.Status` | readOnly | string | Current status of this IPv6 prefix entry. |
| 620 | `Device.IP.Interface.{i}.IPv6Prefix.{i}.ValidLifetime` | readWrite | dateTime | Valid lifetime for the related address or prefix. |
| 621 | `Device.IP.Interface.{i}.IPv6PrefixNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this IP interface. |
| 622 | `Device.IP.Interface.{i}.LastChange` | readOnly | unsignedInt | Seconds since this IP interface last changed state. |
| 623 | `Device.IP.Interface.{i}.Loopback` | readWrite | boolean | Indicates whether this IP interface operates as loopback. |
| 624 | `Device.IP.Interface.{i}.LowerLayers` | readWrite | string | Lower-layer interface references for this IP interface. |
| 625 | `Device.IP.Interface.{i}.MaxMTUSize` | readWrite | unsignedInt | Maximum MTU configured for this IP interface. |
| 626 | `Device.IP.Interface.{i}.Name` | readOnly | string | Name reported for this IP interface. |
| 627 | `Device.IP.Interface.{i}.Reset` | readWrite | boolean | Triggers a reset action for this IP interface. |
| 628 | `Device.IP.Interface.{i}.Router` | readWrite | string | Router reference associated with this IP interface. |
| 629 | `Device.IP.Interface.{i}.Stats.BroadcastPacketsReceived` | readOnly | unsignedLong | Broadcast packets received on this IP interface. |
| 630 | `Device.IP.Interface.{i}.Stats.BroadcastPacketsSent` | readOnly | unsignedLong | Broadcast packets sent on this IP interface. |
| 631 | `Device.IP.Interface.{i}.Stats.BytesReceived` | readOnly | unsignedLong | Total bytes received on this IP interface. |
| 632 | `Device.IP.Interface.{i}.Stats.BytesSent` | readOnly | unsignedLong | Total bytes sent on this IP interface. |
| 633 | `Device.IP.Interface.{i}.Stats.DiscardPacketsReceived` | readOnly | unsignedInt | Received packets discarded on this IP interface. |
| 634 | `Device.IP.Interface.{i}.Stats.DiscardPacketsSent` | readOnly | unsignedInt | Outbound packets discarded on this IP interface. |
| 635 | `Device.IP.Interface.{i}.Stats.ErrorsReceived` | readOnly | unsignedInt | Receive errors seen on this IP interface. |
| 636 | `Device.IP.Interface.{i}.Stats.ErrorsSent` | readOnly | unsignedInt | Transmit errors seen on this IP interface. |
| 637 | `Device.IP.Interface.{i}.Stats.MulticastPacketsReceived` | readOnly | unsignedLong | Multicast packets received on this IP interface. |
| 638 | `Device.IP.Interface.{i}.Stats.MulticastPacketsSent` | readOnly | unsignedLong | Multicast packets sent on this IP interface. |
| 639 | `Device.IP.Interface.{i}.Stats.PacketsReceived` | readOnly | unsignedLong | Total packets received on this IP interface. |
| 640 | `Device.IP.Interface.{i}.Stats.PacketsSent` | readOnly | unsignedLong | Total packets sent on this IP interface. |
| 641 | `Device.IP.Interface.{i}.Stats.UnicastPacketsReceived` | readOnly | unsignedLong | Unicast packets received on this IP interface. |
| 642 | `Device.IP.Interface.{i}.Stats.UnicastPacketsSent` | readOnly | unsignedLong | Unicast packets sent on this IP interface. |
| 643 | `Device.IP.Interface.{i}.Stats.UnknownProtoPacketsReceived` | readOnly | unsignedInt | Packets with unknown protocol received on this IP interface. |
| 644 | `Device.IP.Interface.{i}.Status` | readOnly | string | Current status of this IP interface. |
| 645 | `Device.IP.Interface.{i}.Type` | readOnly | string | Type reported for the related object. |
| 646 | `Device.IP.Interface.{i}.ULAEnable` | readWrite | boolean | Enables or disables ULA addressing on this IP interface. |
| 647 | `Device.IP.InterfaceNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 648 | `Device.IP.ULAPrefix` | readWrite | string | Current ULA prefix configured for the device. |
| 649 | `Device.InterfaceStack.{i}.HigherLayer` | readOnly | string | Higher-layer interface reference in this stack relationship. |
| 650 | `Device.InterfaceStack.{i}.LowerLayer` | readOnly | string | Lower-layer interface reference in this stack relationship. |
| 651 | `Device.InterfaceStackNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 652 | `Device.ManagementServer.AliasBasedAddressing` | readOnly | boolean | Indicates whether alias-based addressing is supported. |
| 653 | `Device.ManagementServer.AutoCreateInstances` | readWrite | boolean | Controls automatic creation of multi-instance objects. |
| 654 | `Device.ManagementServer.CWMPRetryIntervalMultiplier` | readWrite | unsignedInt | CWMP retry timing parameter used by the management client. |
| 655 | `Device.ManagementServer.CWMPRetryMinimumWaitInterval` | readWrite | unsignedInt | CWMP retry timing parameter used by the management client. |
| 656 | `Device.ManagementServer.ConnectionRequestPassword` | writeOnly | string | Credential used by the management server client. |
| 657 | `Device.ManagementServer.ConnectionRequestURL` | readOnly | string | URL used by the ACS to issue connection requests. |
| 658 | `Device.ManagementServer.ConnectionRequestUsername` | readWrite | string | Username used by the management server client. |
| 659 | `Device.ManagementServer.DefaultActiveNotificationThrottle` | readWrite | unsignedInt | Throttle interval for active notifications. |
| 660 | `Device.ManagementServer.DownloadProgressURL` | readOnly | string | URL used to report download progress. |
| 661 | `Device.ManagementServer.EnableCWMP` | readWrite | boolean | Enables or disables CWMP communication. |
| 662 | `Device.ManagementServer.InstanceMode` | readWrite | string | Instance addressing mode used by the management client. |
| 663 | `Device.ManagementServer.KickURL` | readOnly | string | Kick URL exposed by the management client. |
| 664 | `Device.ManagementServer.NATDetected` | readOnly | boolean | Indicates whether NAT is detected for ACS communication. |
| 665 | `Device.ManagementServer.ParameterKey` | readOnly | string | Parameter key associated with the most recent configuration change. |
| 666 | `Device.ManagementServer.Password` | writeOnly | string | Credential used by the management server client. |
| 667 | `Device.ManagementServer.PeriodicInformEnable` | readWrite | boolean | Enables or disables periodic Inform messages. |
| 668 | `Device.ManagementServer.PeriodicInformInterval` | readWrite | unsignedInt | Interval between periodic Inform messages, in seconds. |
| 669 | `Device.ManagementServer.PeriodicInformTime` | readWrite | dateTime | Reference time for scheduling periodic Inform messages. |
| 670 | `Device.ManagementServer.STUNEnable` | readWrite | boolean | Enables or disables STUN for connection requests. |
| 671 | `Device.ManagementServer.STUNMaximumKeepAlivePeriod` | readWrite | int | STUN keepalive timing value used by the management client. |
| 672 | `Device.ManagementServer.STUNMinimumKeepAlivePeriod` | readWrite | unsignedInt | STUN keepalive timing value used by the management client. |
| 673 | `Device.ManagementServer.STUNPassword` | readWrite | string | Shared secret or password used by the management server client. |
| 674 | `Device.ManagementServer.STUNServerAddress` | readWrite | string | STUN server address used by the management client. |
| 675 | `Device.ManagementServer.STUNServerPort` | readWrite | unsignedInt | STUN server port used by the management client. |
| 676 | `Device.ManagementServer.STUNUsername` | readWrite | string | Username used by the management server client. |
| 677 | `Device.ManagementServer.UDPConnectionRequestAddress` | readOnly | string | UDP address used for connection requests. |
| 678 | `Device.ManagementServer.URL` | readWrite | string | URL used by the management server client. |
| 679 | `Device.ManagementServer.UpgradesManaged` | readWrite | boolean | Indicates whether software upgrades are managed by the ACS. |
| 680 | `Device.ManagementServer.Username` | readWrite | string | Username used by the management server client. |
| 681 | `Device.Services.STBService.1.Capabilities.HDMI.SupportedResolutions` | readOnly | list | Display resolutions supported by the related capability or device. |
| 682 | `Device.Services.STBService.1.Capabilities.VideoDecoder.VideoStandards` | readOnly | list | Video standards supported by the decoder capability. |
| 683 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.AudioStandards` | readOnly | list | STB service property for AudioStandards. |
| 684 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Alias` | readOnly | string | User-assigned alias for this MPEG-H Part 2 profile-level entry. |
| 685 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Level` | readOnly | string | Profile level value for this codec capability entry. |
| 686 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.MaximumDecodingCapability` | readOnly | unsignedInt | Maximum decoding capability reported for this codec entry. |
| 687 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1.Profile` | readOnly | string | Profile reported for the related entry. |
| 688 | `Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevelNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the STB video decoder capability set. |
| 689 | `Device.Services.STBService.1.Components.AudioOutput.1.AudioFormat` | readOnly | string | Current audio format reported for this output. |
| 690 | `Device.Services.STBService.1.Components.AudioOutput.1.AudioLevel` | readWrite | unsignedInt | Current audio level for this output. |
| 691 | `Device.Services.STBService.1.Components.AudioOutput.1.CancelMute` | readWrite | boolean | Clears mute state for this audio output when set. |
| 692 | `Device.Services.STBService.1.Components.AudioOutput.1.Enable` | readWrite | boolean | Enables or disables this audio output. |
| 693 | `Device.Services.STBService.1.Components.AudioOutput.1.Name` | readOnly | string | Name reported for this audio output. |
| 694 | `Device.Services.STBService.1.Components.AudioOutput.1.Status` | readOnly | string | Current status of this audio output. |
| 695 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioCompression` | readWrite | string | Audio compression mode configured for this output. |
| 696 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioDB` | readWrite | string | Audio level in dB for this output. |
| 697 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioEncoding` | readWrite | string | Audio encoding mode configured for this output. |
| 698 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioGain` | readWrite | string | Audio gain setting for this output. |
| 699 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioLoopThru` | readWrite | string | Loop-through audio mode for this output. |
| 700 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioOptimalLevel` | readWrite | string | Optimal audio level setting for this output. |
| 701 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_AudioStereoMode` | readWrite | string | Stereo mode configured for this output. |
| 702 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_MaxAudioDB` | readOnly | string | Maximum supported audio level in dB for this output. |
| 703 | `Device.Services.STBService.1.Components.AudioOutput.1.X_COMCAST-COM_MinAudioDB` | readOnly | string | Minimum supported audio level in dB for this output. |
| 704 | `Device.Services.STBService.1.Components.AudioOutputNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the STB service. |
| 705 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.AutoLipSyncSupport` | readOnly | boolean | Indicates whether the connected display supports auto lip-sync. |
| 706 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.CECSupport` | readOnly | boolean | Indicates whether the connected display supports CEC. |
| 707 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.EEDID` | readOnly | string | EDID data reported by the connected HDMI display. |
| 708 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.HDMI3DPresent` | readOnly | boolean | Indicates whether the connected display reports HDMI 3D support. |
| 709 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.PreferredResolution` | readOnly | string | Preferred resolution reported by the connected display. |
| 710 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.Status` | readOnly | string | Current status of the connected HDMI display. |
| 711 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.SupportedResolutions` | readOnly | string | Display resolutions supported by the related capability or device. |
| 712 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.VideoLatency` | readOnly | unsignedInt | Video latency reported by the connected display. |
| 713 | `Device.Services.STBService.1.Components.HDMI.1.DisplayDevice.X_COMCAST-COM_EDID` | readOnly | string | EDID data reported by the connected HDMI display. |
| 714 | `Device.Services.STBService.1.Components.HDMI.1.Enable` | readWrite | boolean | Enables or disables this HDMI output. |
| 715 | `Device.Services.STBService.1.Components.HDMI.1.Name` | readOnly | string | Name reported for this HDMI output. |
| 716 | `Device.Services.STBService.1.Components.HDMI.1.ResolutionMode` | readWrite | string | Resolution selection mode for this HDMI output. |
| 717 | `Device.Services.STBService.1.Components.HDMI.1.ResolutionValue` | readWrite | string | Current resolution value for this HDMI output. |
| 718 | `Device.Services.STBService.1.Components.HDMI.1.Status` | readOnly | string | Current status of this HDMI output. |
| 719 | `Device.Services.STBService.1.Components.HDMINumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the STB service. |
| 720 | `Device.Services.STBService.1.Components.VideoDecoder.1.ContentAspectRatio` | readOnly | string | Current content aspect ratio reported by the video decoder. |
| 721 | `Device.Services.STBService.1.Components.VideoDecoder.1.Enable` | readWrite | boolean | Enables or disables this video decoder. |
| 722 | `Device.Services.STBService.1.Components.VideoDecoder.1.Name` | readOnly | string | Name reported for this video decoder. |
| 723 | `Device.Services.STBService.1.Components.VideoDecoder.1.Status` | readOnly | string | Current status of this video decoder. |
| 724 | `Device.Services.STBService.1.Components.VideoDecoder.1.X_COMCAST-COM_Standby` | readWrite | boolean | Standby state for this video decoder. |
| 725 | `Device.Services.STBService.1.Components.VideoDecoder.1.X_RDKCENTRAL-COM_MPEGHPart2` | readOnly | string | MPEG-H Part 2 capability string reported by the decoder. |
| 726 | `Device.Services.STBService.1.Components.VideoDecoderNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the STB service. |
| 727 | `Device.Services.STBService.1.Components.VideoOutput.1.AspectRatioBehaviour` | readWrite | string | Aspect-ratio handling mode for this video output. |
| 728 | `Device.Services.STBService.1.Components.VideoOutput.1.DisplayFormat` | readWrite | string | Display format configured for this video output. |
| 729 | `Device.Services.STBService.1.Components.VideoOutput.1.Enable` | readWrite | boolean | Enables or disables this video output. |
| 730 | `Device.Services.STBService.1.Components.VideoOutput.1.HDCP` | readWrite | boolean | HDCP state configured for this video output. |
| 731 | `Device.Services.STBService.1.Components.VideoOutput.1.Name` | readOnly | string | Name reported for this video output. |
| 732 | `Device.Services.STBService.1.Components.VideoOutput.1.Status` | readOnly | string | Current status of this video output. |
| 733 | `Device.Services.STBService.1.Components.VideoOutput.1.VideoFormat` | readWrite | string | Video format configured for this video output. |
| 734 | `Device.Services.STBService.1.Components.VideoOutputNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the STB service. |
| 735 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.BatteryLevelLoaded` | readOnly | unsignedInt | Battery metric reported for this RF4CE remote. |
| 736 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.BatteryLevelUnloaded` | readOnly | unsignedInt | Battery metric reported for this RF4CE remote. |
| 737 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.BatteryPercentage` | readOnly | unsignedInt | Battery metric reported for this RF4CE remote. |
| 738 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.BatteryReplacement` | readOnly | boolean | Indicates whether the RF4CE remote battery should be replaced. |
| 739 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.ImpendingDoom` | readOnly | boolean | Indicates whether the RF4CE remote reports a critical battery condition. |
| 740 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.LinkQuality` | readOnly | unsignedInt | Link quality reported for this RF4CE remote. |
| 741 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.MACAddress` | readOnly | string | MAC address associated with this RF4CE remote. |
| 742 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.NetworkAddress` | readOnly | unsignedInt | Network address reported for the related RF4CE object. |
| 743 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.RemoteId` | readOnly | unsignedInt | Remote identifier reported for this RF4CE remote. |
| 744 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.RemoteType` | readOnly | string | Remote type reported for this RF4CE remote. |
| 745 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.SignalStrength` | readOnly | int | Signal strength reported for the related entry. |
| 746 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.VersionInfoHW` | readOnly | string | Version information reported for the related RF4CE object. |
| 747 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.Remote.1.VersionInfoSW` | readOnly | string | Version information reported for the related RF4CE object. |
| 748 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4ceActiveChannel` | readOnly | unsignedInt | RF4CE network property reported by the subsystem. |
| 749 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4ceMACAddress` | readOnly | string | RF4CE network property reported by the subsystem. |
| 750 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4ceNetworkAddress` | readOnly | unsignedInt | RF4CE network property reported by the subsystem. |
| 751 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4cePANID` | readOnly | unsignedInt | RF4CE network property reported by the subsystem. |
| 752 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4cePairedRemotesNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in the RF4CE subsystem. |
| 753 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_RF4CE.rf4ceVersionInfo` | readOnly | string | Version information reported for the related RF4CE object. |
| 754 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.Capacity` | readOnly | unsignedInt | SD card storage property: Capacity. |
| 755 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.CardFailed` | readOnly | boolean | SD card storage property: CardFailed. |
| 756 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.LifeElapsed` | readOnly | int | SD card storage property: LifeElapsed. |
| 757 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.LotID` | readOnly | string | SD card storage property: LotID. |
| 758 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.Manufacturer` | readOnly | string | SD card storage property: Manufacturer. |
| 759 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.Model` | readOnly | string | SD card storage property: Model. |
| 760 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.ReadOnly` | readOnly | boolean | SD card storage property: ReadOnly. |
| 761 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.SerialNumber` | readOnly | int | SD card storage property: SerialNumber. |
| 762 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.Status` | readOnly | string | SD card storage property: Status. |
| 763 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.TSBQualified` | readOnly | boolean | SD card storage property: TSBQualified. |
| 764 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.Capacity` | readOnly | unsignedInt | Storage capacity reported for the device. |
| 765 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.DeviceReport` | readOnly | string | Detailed health report for the storage device. |
| 766 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.FirmwareVersion` | readOnly | string | Version string reported for the eMMC flash device. |
| 767 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.LifeElapsedA` | readOnly | int | Wear indicator reported for the storage device. |
| 768 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.LifeElapsedB` | readOnly | int | Wear indicator reported for the storage device. |
| 769 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.LotID` | readOnly | string | Manufacturing lot identifier for the device. |
| 770 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.Manufacturer` | readOnly | string | Manufacturer reported for the eMMC flash device. |
| 771 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.Model` | readOnly | string | Model identifier reported for the eMMC flash device. |
| 772 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.PreEOLStateEUDA` | readOnly | string | Pre-EOL health state for the named storage area. |
| 773 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.PreEOLStateMLC` | readOnly | string | Pre-EOL health state for the named storage area. |
| 774 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.PreEOLStateSystem` | readOnly | string | Pre-EOL health state for the named storage area. |
| 775 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.ReadOnly` | readOnly | boolean | Indicates whether the device is operating in read-only mode. |
| 776 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.SerialNumber` | readOnly | string | Serial number reported for the eMMC flash device. |
| 777 | `Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash.TSBQualified` | readOnly | boolean | Indicates whether the storage device is qualified for TSB use. |
| 778 | `Device.Services.STBService.1.Enable` | readWrite | boolean | Enables or disables the STB service. |
| 779 | `Device.Services.STBServiceNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 780 | `Device.Time.Chrony.Enable` | readWrite | boolean | Enables or disables the related feature. |
| 781 | `Device.Time.Chrony.Makestep` | readWrite | string | Chrony time synchronization setting for Makestep. |
| 782 | `Device.Time.Chrony.NTPServer.{i}.Settings` | readWrite | string | Chrony time synchronization setting for Settings. |
| 783 | `Device.Time.CurrentLocalTime` | readWrite | string | Current local time reported by the device. |
| 784 | `Device.Time.Enable` | readWrite | boolean | Enables or disables the system time service. |
| 785 | `Device.Time.LocalTimeZone` | readWrite | string | Current local timezone setting reported by the device. |
| 786 | `Device.Time.NTPServer1` | readWrite | string | Configured NTP server address for the numbered slot. |
| 787 | `Device.Time.NTPServer2` | readWrite | string | Configured NTP server address for the numbered slot. |
| 788 | `Device.Time.NTPServer3` | readWrite | string | Configured NTP server address for the numbered slot. |
| 789 | `Device.Time.NTPServer4` | readWrite | string | Configured NTP server address for the numbered slot. |
| 790 | `Device.Time.NTPServer5` | readWrite | string | Configured NTP server address for the numbered slot. |
| 791 | `Device.Time.Status` | readWrite | string | Current status of the system time service. |
| 792 | `Device.Time.X_RDK_CurrentUTCTime` | readOnly | string | Current UTC time reported by the device. |
| 793 | `Device.WiFi.AccessPoint.{i}.Alias` | readWrite | string | User-assigned alias for this Wi-Fi access point. |
| 794 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.Active` | readOnly | boolean | Indicates whether the related entry is currently active. |
| 795 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.AuthenticationState` | readOnly | boolean | Configuration or status value for this associated Wi-Fi client. |
| 796 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.LastDataDownlinkRate` | readOnly | unsignedInt | Configuration or status value for this associated Wi-Fi client. |
| 797 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.LastDataUplinkRate` | readOnly | unsignedInt | Configuration or status value for this associated Wi-Fi client. |
| 798 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.MACAddress` | readOnly | string | MAC address associated with this associated Wi-Fi client. |
| 799 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.Retransmissions` | readOnly | unsignedInt | Configuration or status value for this associated Wi-Fi client. |
| 800 | `Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.SignalStrength` | readOnly | int | Signal strength reported for the related entry. |
| 801 | `Device.WiFi.AccessPoint.{i}.AssociatedDeviceNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this Wi-Fi access point. |
| 802 | `Device.WiFi.AccessPoint.{i}.Enable` | readWrite | boolean | Enables or disables this Wi-Fi access point. |
| 803 | `Device.WiFi.AccessPoint.{i}.RetryLimit` | readWrite | unsignedInt | Configuration or status value for this Wi-Fi access point. |
| 804 | `Device.WiFi.AccessPoint.{i}.SSIDAdvertisementEnabled` | readWrite | boolean | Controls whether this access point advertises its SSID. |
| 805 | `Device.WiFi.AccessPoint.{i}.SSIDReference` | readWrite | string | Reference to the SSID object used by this entry. |
| 806 | `Device.WiFi.AccessPoint.{i}.Security.KeyPassphrase` | readWrite | string | Passphrase configured for the related Wi-Fi profile. |
| 807 | `Device.WiFi.AccessPoint.{i}.Security.ModeEnabled` | readWrite | string | Security mode currently enabled for the related Wi-Fi object. |
| 808 | `Device.WiFi.AccessPoint.{i}.Security.ModesSupported` | readOnly | string | Security modes supported by the related Wi-Fi object. |
| 809 | `Device.WiFi.AccessPoint.{i}.Security.PreSharedKey` | readWrite | hexBinary | Pre-shared key configured for the related Wi-Fi object. |
| 810 | `Device.WiFi.AccessPoint.{i}.Security.RadiusSecret` | readWrite | string | Shared secret or password used by this Wi-Fi access point. |
| 811 | `Device.WiFi.AccessPoint.{i}.Security.RadiusServerIPAddr` | readWrite | string | RADIUS server IP address used by this access point. |
| 812 | `Device.WiFi.AccessPoint.{i}.Security.RadiusServerPort` | readWrite | unsignedInt | Port value used by this Wi-Fi access point. |
| 813 | `Device.WiFi.AccessPoint.{i}.Security.RekeyingInterval` | readWrite | unsignedInt | Key rekey interval for this Wi-Fi security profile. |
| 814 | `Device.WiFi.AccessPoint.{i}.Security.WEPKey` | readWrite | hexBinary | WEP key configured for the related Wi-Fi object. |
| 815 | `Device.WiFi.AccessPoint.{i}.Status` | readOnly | string | Current status of this Wi-Fi access point. |
| 816 | `Device.WiFi.AccessPoint.{i}.UAPSDCapability` | readOnly | boolean | U-APSD capability or enable state for this access point. |
| 817 | `Device.WiFi.AccessPoint.{i}.UAPSDEnable` | readWrite | boolean | U-APSD capability or enable state for this access point. |
| 818 | `Device.WiFi.AccessPoint.{i}.WMMCapability` | readOnly | boolean | WMM capability or enable state for this access point. |
| 819 | `Device.WiFi.AccessPoint.{i}.WMMEnable` | readWrite | boolean | WMM capability or enable state for this access point. |
| 820 | `Device.WiFi.AccessPoint.{i}.WPS.ConfigMethodsEnabled` | readWrite | string | WPS configuration methods enabled or supported for the related Wi-Fi object. |
| 821 | `Device.WiFi.AccessPoint.{i}.WPS.ConfigMethodsSupported` | readOnly | string | WPS configuration methods enabled or supported for the related Wi-Fi object. |
| 822 | `Device.WiFi.AccessPoint.{i}.WPS.Enable` | readWrite | boolean | Enables or disables this Wi-Fi access point. |
| 823 | `Device.WiFi.AccessPointNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 824 | `Device.WiFi.EndPoint.{i}.Alias` | readWrite | string | User-assigned alias for this Wi-Fi endpoint. |
| 825 | `Device.WiFi.EndPoint.{i}.Enable` | readWrite | boolean | Enables or disables this Wi-Fi endpoint. |
| 826 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Alias` | readWrite | string | User-assigned alias for this Wi-Fi endpoint profile. |
| 827 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Enable` | readWrite | boolean | Enables or disables this Wi-Fi endpoint profile. |
| 828 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Location` | readWrite | string | Location hint associated with this Wi-Fi endpoint profile. |
| 829 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Priority` | readWrite | unsignedInt | Scheduling priority for this Wi-Fi endpoint profile. |
| 830 | `Device.WiFi.EndPoint.{i}.Profile.{i}.SSID` | readWrite | string | SSID string used by the related Wi-Fi object. |
| 831 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Security.KeyPassphrase` | readWrite | string | Passphrase configured for the related Wi-Fi profile. |
| 832 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Security.ModeEnabled` | readWrite | string | Security mode currently enabled for the related Wi-Fi object. |
| 833 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Security.PreSharedKey` | readWrite | hexBinary | Pre-shared key configured for the related Wi-Fi object. |
| 834 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Security.WEPKey` | readWrite | hexBinary | WEP key configured for the related Wi-Fi object. |
| 835 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Status` | readOnly | string | Current status of this Wi-Fi endpoint profile. |
| 836 | `Device.WiFi.EndPoint.{i}.ProfileNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this Wi-Fi endpoint. |
| 837 | `Device.WiFi.EndPoint.{i}.ProfileReference` | readWrite | string | Reference to the active Wi-Fi endpoint profile. |
| 838 | `Device.WiFi.EndPoint.{i}.SSIDReference` | readOnly | string | Reference to the SSID object used by this entry. |
| 839 | `Device.WiFi.EndPoint.{i}.Security.ModesEnabled` | readOnly | string | Security mode currently enabled for the related Wi-Fi object. |
| 840 | `Device.WiFi.EndPoint.{i}.Security.ModesSupported` | readOnly | string | Security modes supported by the related Wi-Fi object. |
| 841 | `Device.WiFi.EndPoint.{i}.Stats.LastDataDownlinkRate` | readOnly | unsignedInt | Most recent downlink data rate for this Wi-Fi endpoint. |
| 842 | `Device.WiFi.EndPoint.{i}.Stats.LastDataUplinkRate` | readOnly | unsignedInt | Most recent uplink data rate for this Wi-Fi endpoint. |
| 843 | `Device.WiFi.EndPoint.{i}.Stats.Retransmissions` | readOnly | unsignedInt | Retransmission count observed for this Wi-Fi endpoint. |
| 844 | `Device.WiFi.EndPoint.{i}.Stats.SignalStrength` | readOnly | int | Reported signal strength for this Wi-Fi endpoint. |
| 845 | `Device.WiFi.EndPoint.{i}.Status` | readOnly | string | Current status of this Wi-Fi endpoint. |
| 846 | `Device.WiFi.EndPoint.{i}.WPS.ConfigMethodsEnabled` | readWrite | string | WPS configuration methods enabled or supported for the related Wi-Fi object. |
| 847 | `Device.WiFi.EndPoint.{i}.WPS.ConfigMethodsSupported` | readOnly | string | WPS configuration methods enabled or supported for the related Wi-Fi object. |
| 848 | `Device.WiFi.EndPoint.{i}.WPS.Enable` | readWrite | boolean | Enables or disables this Wi-Fi endpoint. |
| 849 | `Device.WiFi.EndPointNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 850 | `Device.WiFi.Radio.{i}.OperatingChannelBandwidth` | readOnly | string | Current operating channel bandwidth of this Wi-Fi radio. |
| 851 | `Device.WiFi.Radio.{i}.Stats.Noise` | readOnly | int | Reported noise floor for this Wi-Fi radio. |
| 852 | `Device.WiFi.Radio.{i}.Stats.PacketsReceived` | readOnly | unsignedLong | Total packets received on this Wi-Fi radio. |
| 853 | `Device.WiFi.RadioNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 854 | `Device.WiFi.SSID.{i}.Alias` | readWrite | string | User-assigned alias for this SSID interface. |
| 855 | `Device.WiFi.SSID.{i}.BSSID` | readOnly | string | BSSID reported for this SSID interface. |
| 856 | `Device.WiFi.SSID.{i}.Enable` | readWrite | boolean | Enables or disables this SSID interface. |
| 857 | `Device.WiFi.SSID.{i}.LastChange` | readOnly | unsignedInt | Seconds since this SSID interface last changed state. |
| 858 | `Device.WiFi.SSID.{i}.LowerLayers` | readWrite | string | Lower-layer interface references for this SSID interface. |
| 859 | `Device.WiFi.SSID.{i}.MACAddress` | readOnly | string | MAC address associated with this SSID interface. |
| 860 | `Device.WiFi.SSID.{i}.Name` | readOnly | string | Name reported for this SSID interface. |
| 861 | `Device.WiFi.SSID.{i}.SSID` | readWrite | string | SSID string used by the related Wi-Fi object. |
| 862 | `Device.WiFi.SSID.{i}.Stats.BroadcastPacketsReceived` | readOnly | unsignedLong | Broadcast packets received on this SSID interface. |
| 863 | `Device.WiFi.SSID.{i}.Stats.BroadcastPacketsSent` | readOnly | unsignedLong | Broadcast packets sent on this SSID interface. |
| 864 | `Device.WiFi.SSID.{i}.Stats.BytesReceived` | readOnly | unsignedLong | Total bytes received on this SSID interface. |
| 865 | `Device.WiFi.SSID.{i}.Stats.BytesSent` | readOnly | unsignedLong | Total bytes sent on this SSID interface. |
| 866 | `Device.WiFi.SSID.{i}.Stats.DiscardPacketsReceived` | readOnly | unsignedInt | Received packets discarded on this SSID interface. |
| 867 | `Device.WiFi.SSID.{i}.Stats.DiscardPacketsSent` | readOnly | unsignedInt | Outbound packets discarded on this SSID interface. |
| 868 | `Device.WiFi.SSID.{i}.Stats.ErrorsReceived` | readOnly | unsignedInt | Receive errors seen on this SSID interface. |
| 869 | `Device.WiFi.SSID.{i}.Stats.ErrorsSent` | readOnly | unsignedInt | Transmit errors seen on this SSID interface. |
| 870 | `Device.WiFi.SSID.{i}.Stats.MulticastPacketsReceived` | readOnly | unsignedLong | Multicast packets received on this SSID interface. |
| 871 | `Device.WiFi.SSID.{i}.Stats.MulticastPacketsSent` | readOnly | unsignedLong | Multicast packets sent on this SSID interface. |
| 872 | `Device.WiFi.SSID.{i}.Stats.PacketsReceived` | readOnly | unsignedLong | Total packets received on this SSID interface. |
| 873 | `Device.WiFi.SSID.{i}.Stats.PacketsSent` | readOnly | unsignedLong | Total packets sent on this SSID interface. |
| 874 | `Device.WiFi.SSID.{i}.Stats.UnicastPacketsReceived` | readOnly | unsignedLong | Unicast packets received on this SSID interface. |
| 875 | `Device.WiFi.SSID.{i}.Stats.UnicastPacketsSent` | readOnly | unsignedLong | Unicast packets sent on this SSID interface. |
| 876 | `Device.WiFi.SSID.{i}.Stats.UnknownProtoPacketsReceived` | readOnly | unsignedInt | Packets with unknown protocol received on this SSID interface. |
| 877 | `Device.WiFi.SSID.{i}.Status` | readOnly | string | Current status of this SSID interface. |
| 878 | `Device.WiFi.SSIDNumberOfEntries` | readOnly | unsignedInt | Reports the number of entries in this object. |
| 879 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.80211kvrEnable` | readWrite | boolean | Enables or disables 802.11k/v/r roaming support. |
| 880 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.Enable` | readWrite | boolean | Enables or disables the Wi-Fi client roaming policy. |
| 881 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_APcontrolBeaconsMissedTime` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 882 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_APcontrolThresholdLevel` | readWrite | int | Band-steering threshold or control used by the client roaming policy. |
| 883 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_APcontrolTimeframe` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 884 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BackOffTime` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 885 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BestDeltaLevelConnected` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 886 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_BestDeltaLevelDisconnected` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 887 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerBeaconsMissedTime` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 888 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerThresholdLevel` | readWrite | int | Band-steering threshold or control used by the client roaming policy. |
| 889 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PostAssn_SelfSteerTimeframe` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 890 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PreAssn_BestDeltaLevel` | readWrite | unsignedInt | Band-steering threshold or control used by the client roaming policy. |
| 891 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.PreAssn_BestThresholdLevel` | readWrite | int | Band-steering threshold or control used by the client roaming policy. |
| 892 | `Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.SelfSteer_OverrideEnable` | readWrite | boolean | Band-steering threshold or control used by the client roaming policy. |
| 893 | `Device.WiFi.X_RDKCENTRAL-COM_WiFiEnable` | readWrite | boolean | Master enable for the Wi-Fi subsystem. |
| 894 | `Device.X_COMCAST-COM_Xcalibur.Client.CoPilot.rssServerArgs` | readWrite | string | CoPilot client configuration value for rssServerArgs. |
| 895 | `Device.X_COMCAST-COM_Xcalibur.Client.CoPilot.rssServerMode` | readWrite | unsignedInt | CoPilot client configuration value for rssServerMode. |
| 896 | `Device.X_COMCAST-COM_Xcalibur.Client.SecuritySystem.ssDeviceId` | readOnly | string | Security system device identifier. |
| 897 | `Device.X_COMCAST-COM_Xcalibur.Client.SecuritySystem.ssDeviceReg` | readOnly | dateTime | Security system device registration time. |
| 898 | `Device.X_COMCAST-COM_Xcalibur.Client.SecuritySystem.ssErrorCnt` | readOnly | unsignedInt | Security system error count. |
| 899 | `Device.X_COMCAST-COM_Xcalibur.Client.SecuritySystem.ssRegTs` | readOnly | boolean | Indicates whether a security system registration timestamp is available. |
| 900 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreAppId` | readOnly | string | Application identifier for this XRE connection entry. |
| 901 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnEstTs` | readOnly | string | Connection establishment timestamp for this XRE connection entry. |
| 902 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnIfName` | readOnly | string | Interface name used by this XRE connection entry. |
| 903 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnRetryAttempts` | readOnly | unsignedInt | Retry attempts recorded for this XRE connection entry. |
| 904 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnStatus` | readOnly | string | Current status of this XRE connection entry. |
| 905 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnURL` | readOnly | string | Connection URL used by this XRE connection entry. |
| 906 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreAvgCmdProcTime` | readOnly | int | Command processing time statistic reported by XRE. |
| 907 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreChannelMapId` | readOnly | string | Channel map identifier currently used by the XRE client. |
| 908 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreCommandCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 909 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreControllerId` | readOnly | string | Controller identifier reported by the XRE client. |
| 910 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreEnable` | readWrite | boolean | Enables or disables the XRE client. |
| 911 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreErrorCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 912 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreFlushLocalCache` | readWrite | boolean | Triggers an XRE local cache flush when set. |
| 913 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreGatewaySTBMAC` | readOnly | string | Gateway STB MAC address reported by the XRE client. |
| 914 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreGetTWPDiags` | readOnly | string | Diagnostic payload returned by XRE TWP diagnostics. |
| 915 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreLastURLAccessed` | readOnly | string | Last URL accessed by the XRE client. |
| 916 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreLastVideoUrl` | readOnly | string | Last video URL accessed by the XRE client. |
| 917 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreLogLevel` | readWrite | string | Logging level used by the XRE client. |
| 918 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreMaxCmdProcTime` | readOnly | int | Command processing time statistic reported by XRE. |
| 919 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreMinCmdProcTime` | readOnly | int | Command processing time statistic reported by XRE. |
| 920 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xrePlantId` | readOnly | string | Plant identifier reported by the XRE client. |
| 921 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreReceiverId` | readOnly | string | Receiver identifier reported by the XRE client. |
| 922 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreRefreshXreSession` | readWrite | boolean | Triggers XRE session refresh behavior. |
| 923 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreRefreshXreSessionWithRR` | readWrite | int | Controls refresh-with-RR behavior for the XRE session. |
| 924 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreRestartXreApp` | writeOnly | string | XRE client value for xreRestartXreApp. |
| 925 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreSessionId` | readOnly | string | Active XRE session identifier reported by the client. |
| 926 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreSessionLastModTs` | readOnly | string | Timestamp of the last XRE session update. |
| 927 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreSessionUptime` | readOnly | string | Uptime of the current XRE session. |
| 928 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreStatus` | readOnly | string | Configuration or status value for the XRE client. |
| 929 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotAnimCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 930 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotAppCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 931 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotFlashCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 932 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotFontCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 933 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotHtmlTxtCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 934 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotImgCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 935 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotNineSliceImgCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 936 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotRectCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 937 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotSoundCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 938 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotStyleshtCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 939 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotTxtCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 940 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotTxtIpCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 941 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotVideoCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 942 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotViewCnt` | readOnly | unsignedInt | Count metric reported by the XRE client. |
| 943 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreVersion` | readOnly | string | Version string reported by the XRE client. |
| 944 | `Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreVodId` | readOnly | string | VOD identifier reported by the XRE client. |
| 945 | `Device.X_COMCAST-COM_Xcalibur.Client.xconfCheckNow` | readWrite | string | Triggers an immediate Xconf check for the Xcalibur client. |
| 946 | `Device.X_COMCAST-COM_Xcalibur.DevApp.devAppNumAps` | readOnly | unsignedInt | Number of DevApp application entries reported by the platform. |
| 947 | `Device.X_COMCAST-COM_Xcalibur.DevApp.devAppTable.{i}.devAppId` | readOnly | string | Application identifier for this DevApp entry. |
| 948 | `Device.X_COMCAST-COM_Xcalibur.DevApp.devAppTable.{i}.devAppRestartCapability` | readOnly | string | Restart capability reported for this DevApp entry. |
| 949 | `Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewayDeviceFriendlyName` | readOnly | string | Gateway identification value reported by TRM. |
| 950 | `Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewayMoCAIP` | readOnly | string | Gateway identification value reported by TRM. |
| 951 | `Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewayMoCAMAC` | readOnly | string | Gateway identification value reported by TRM. |
| 952 | `Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewaySTBMAC` | readOnly | string | Gateway identification value reported by TRM. |
| 953 | `Device.X_RDKCENTRAL-COM_T2.ReportProfiles` | readWrite | string | Telemetry 2.0 report profiles payload. |
| 954 | `Device.X_RDKCENTRAL-COM_T2.ReportProfilesMsgPack` | readWrite | string | Telemetry 2.0 report profiles payload. |
| 955 | `Device.X_RDK_WebPA_DNSText.URL` | readWrite | string | Bootstrap URL used to retrieve WebPA DNS text records. |
| 956 | `Device.X_RDK_WebPA_Server.URL` | readOnly | string | Current WebPA server URL from the bootstrap store. |
| 957 | `Device.X_RDK_WebPA_TokenServer.URL` | readOnly | string | Current WebPA token server URL from the bootstrap store. |

---

## Appendix A: Parameters with Bootstrap Update (`bsUpdate`)

These parameters participate in the bootstrap configuration workflow. The `bsUpdate` field
controls when a bootstrap-sourced value is applied to the device data model.

| # | Parameter | bsUpdate | Data Type | Access | Source File(s) |
|---|-----------|----------|-----------|--------|----------------|
| 1 | `Device.DeviceInfo.X_RDKCENTRAL-COM_Manufacturing.UIbranding` | allUpdate | string | readWrite | data-model-generic.xml |
| 2 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDB` | allUpdate | boolean | readWrite | data-model-generic.xml |
| 3 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDBEnd` | allUpdate | boolean | readWrite | data-model-generic.xml |
| 4 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.NetflixESNprefix` | allUpdate | string | readWrite | data-model-generic.xml |
| 5 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.OsClass` | allUpdate | string | readWrite | data-model-generic.xml |
| 6 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerName` | allUpdate | string | readWrite | data-model-generic.xml |
| 7 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.PartnerProductName` | allUpdate | string | readWrite | data-model-generic.xml |
| 8 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.SsrUrl` | allUpdate | string | readWrite | data-model-generic.xml |
| 9 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.XconfRecoveryUrl` | allUpdate | string | readWrite | data-model-generic.xml |
| 10 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.XconfUrl` | allUpdate | string | readWrite | data-model-generic.xml |
| 11 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CrashportalEndpoint.Enable` | allUpdate | string | readWrite | data-model-generic.xml |
| 12 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CrashportalEndpoint.URL` | allUpdate | string | readWrite | data-model-generic.xml |
| 13 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DefaultSupportedLocales` | allUpdate | string | readWrite | data-model-generic.xml |
| 14 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IPControl.Service.Discovery.Enable` | allUpdate | boolean | readWrite | data-model-generic.xml |
| 15 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IPControl.Subsystem.RICS.Enable` | allUpdate | boolean | readWrite | data-model-generic.xml |
| 16 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.AdCacheEnable` | allUpdate | string | readWrite | data-model-stb.xml |
| 17 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.ByteRangeDownload` | allUpdate | string | readWrite | data-model-stb.xml |
| 18 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.Enable` | allUpdate | string | readWrite | data-model-stb.xml |
| 19 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.PSNUrl` | allUpdate | string | readWrite | data-model-stb.xml |
| 20 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.PlacementReqUrl` | allUpdate | string | readWrite | data-model-stb.xml |
| 21 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LSA.ProgrammerEnable` | allUpdate | string | readWrite | data-model-stb.xml |
| 22 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LogUploadEndpoint.Enable` | allUpdate | string | readWrite | data-model-generic.xml |
| 23 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LogUploadEndpoint.URL` | allUpdate | string | readWrite | data-model-generic.xml |
| 24 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Miracast.Enable` | allUpdate | boolean | readWrite | data-model-generic.xml |
| 25 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ThreadPrioConfig.Enable` | allUpdate | boolean | readWrite | data-model-generic.xml |
| 26 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UNII3.Enable` | allUpdate | boolean | readWrite | data-model-stb.xml |
| 27 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.XDial.AppList` | allUpdate | string | readWrite | data-model-generic.xml |
| 28 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.SystemServices.FriendlyName` | allUpdate | string | readWrite | data-model-generic.xml |
| 29 | `Device.Time.NTPServer1` | allUpdate | string | readWrite | data-model-generic.xml |
| 30 | `Device.Time.NTPServer2` | allUpdate | string | readWrite | data-model-generic.xml |
| 31 | `Device.Time.NTPServer3` | allUpdate | string | readWrite | data-model-generic.xml |
| 32 | `Device.Time.NTPServer4` | allUpdate | string | readWrite | data-model-generic.xml |
| 33 | `Device.Time.NTPServer5` | allUpdate | string | readWrite | data-model-generic.xml |
| 34 | `Device.X_RDKCENTRAL-COM_T2.ReportProfiles` | allUpdate | string | readWrite | data-model-generic.xml |
| 35 | `Device.X_RDKCENTRAL-COM_T2.ReportProfilesMsgPack` | allUpdate | string | readWrite | data-model-generic.xml |
| 36 | `Device.X_RDK_WebPA_DNSText.URL` | allUpdate | string | readWrite | data-model-generic.xml |
| 37 | `Device.X_RDK_WebPA_Server.URL` | allUpdate | string | readOnly | data-model-generic.xml |
| 38 | `Device.X_RDK_WebPA_TokenServer.URL` | allUpdate | string | readOnly | data-model-generic.xml |

---

## Appendix B: Parameters with Factory Default Values

These parameters have a factory default value declared in the XML data model.
When the device is reset to factory settings, these values are applied.

| # | Parameter | Factory Default | Data Type | Access | Source File(s) |
|---|-----------|-----------------|-----------|--------|----------------|
| 1 | `Device.DeviceInfo.Description` | `TR-181, TR-135 and Comcast specific Datamodel Configuration` | string | readOnly | data-model-stb.xml, data-model-tv.xml |
| 2 | `Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 3 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DisplaySettings.AudioPort.HDMI.Enabled` | `FALSE` | boolean | readWrite | data-model-tv.xml |
| 4 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DisplaySettings.AudioPort.HDMI_ARC.Enabled` | `FALSE` | boolean | readWrite | data-model-tv.xml |
| 5 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DisplaySettings.AudioPort.SPDIF.Enabled` | `FALSE` | boolean | readWrite | data-model-tv.xml |
| 6 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DisplaySettings.AudioPort.SPEAKER.Enabled` | `TRUE` | boolean | readWrite | data-model-tv.xml |
| 7 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.DynamicIUIUpdate.Pathway.Uri` | `base64:eyJ2ZXJiIjoiUE9TVCIsInVybCI6Imh0dHBzOi8vZXh0ZXJuYWwucHdydWxlcy5za3kuY29tL3BhdGhjYXQifQ==` | string | readWrite | data-model-generic.xml |
| 8 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppHibernate.DelayInSecLaunchedToSuspended` | `30` | unsignedInt | readWrite | data-model-generic.xml |
| 9 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppHibernate.DelayInSecResumedToSuspended` | `30` | unsignedInt | readWrite | data-model-generic.xml |
| 10 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AppHibernate.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 11 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTR.DebugMode.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 12 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.BTR.GamePad.Enable` | `TRUE` | boolean | readWrite | data-model-generic.xml |
| 13 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DAB.Enable` | `FALSE` | boolean | readWrite | data-model-stb.xml |
| 14 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DHCPv6Client.Enable` | `TRUE` | boolean | readWrite | data-model-generic.xml |
| 15 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HdmiCecSink.CECVersion` | `1.4` | string | readWrite | data-model-generic.xml |
| 16 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.IncrementalCDL.Enable` | `TRUE` | boolean | readWrite | data-model-generic.xml |
| 17 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LaunchDarkly.EnvKeyLabel` | `PROD` | string | readWrite | data-model-generic.xml |
| 18 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LnFUseXPKI.Enable` | `TRUE` | boolean | readWrite | data-model-stb.xml |
| 19 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MOCASSH.Enable` | `TRUE` | boolean | readWrite | data-model-generic.xml |
| 20 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Miracast.Enable` | `TRUE` | boolean | readWrite | data-model-generic.xml |
| 21 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonPersistent.WebPACDL.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 22 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Power.UserInactivityNotification.TimeMinutes` | `15` | unsignedInt | readWrite | data-model-generic.xml |
| 23 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 24 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RedRecovery.Status` | `DISABLED` | string | readWrite | data-model-generic.xml |
| 25 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Resourcemanager.Blacklist.Enable` | `TRUE` | boolean | readWrite | data-model-stb.xml, data-model-tv.xml |
| 26 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Resourcemanager.ReserveTTS.Enable` | `TRUE` | boolean | readWrite | data-model-generic.xml |
| 27 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SHORTS.Enable` | `TRUE` | boolean | readWrite | data-model-generic.xml |
| 28 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.STAGE.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 29 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLDirect.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 30 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 31 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.LowSpeed` | `12800` | int | readWrite | data-model-generic.xml |
| 32 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SWDLSpLimit.TopSpeed` | `1280000` | int | readWrite | data-model-generic.xml |
| 33 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SecDump.Enable` | `TRUE` | boolean | readWrite | data-model-generic.xml |
| 34 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StopMaintenance.Enable` | `TRUE` | boolean | readWrite | data-model-tv.xml |
| 35 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StorageManager.maxAdaptiveBitRate` | `18` | int | readWrite | data-model-generic.xml |
| 36 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.StorageManager.maxTSBDurationMinutes` | `0` | int | readWrite | data-model-generic.xml |
| 37 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.TR069support.Enable` | `FALSE` | boolean | readWrite | data-model-stb.xml, data-model-tv.xml |
| 38 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Telemetry.FTUEReport.Enable` | `FALSE` | boolean | readWrite | data-model-tv.xml |
| 39 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ThreadMonitorMinidump.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 40 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ThreadPrioConfig.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 41 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.TvSettings.DynamicAutoLatency` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 42 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UploadLogsOnUnscheduledReboot.Disable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 43 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.VideoTelemetry.FrequncyMinutes` | `15` | unsignedInt | readWrite | data-model-tv.xml |
| 44 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFiReset.ReassociateTolerance` | `20` | unsignedInt | readWrite | data-model-generic.xml |
| 45 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WifiOptimizer.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 46 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.collectd.PortNumber` | `2003` | unsignedInt | readWrite | data-model-generic.xml |
| 47 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.fkpskdf.Enable` | `TRUE` | boolean | readWrite | data-model-generic.xml |
| 48 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.memcapture.Duration` | `30` | int | readWrite | data-model-generic.xml |
| 49 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.memcapture.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 50 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.meminsight.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 51 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.processmonitor.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 52 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Identity.DbgServices.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 53 | `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Identity.DeviceType` | `PROD` | string | readWrite | data-model-generic.xml |
| 54 | `Device.DeviceInfo.X_RDK_RDKProfileName` | `RDKV` | string | readWrite | data-model-generic.xml |
| 55 | `Device.Ethernet.Link.{i}.Enable` | `0` | boolean | readWrite | data-model-generic.xml |
| 56 | `Device.Ethernet.Link.{i}.Status` | `Down` | string | readOnly | data-model-generic.xml |
| 57 | `Device.IP.Diagnostics.DownloadDiagnostics.DiagnosticsState` | `None` | string | readWrite | data-model-generic.xml |
| 58 | `Device.IP.Diagnostics.DownloadDiagnostics.DownloadTransports` | `HTTP,FTP` | string | readOnly | data-model-generic.xml |
| 59 | `Device.IP.Diagnostics.IPPing.DiagnosticsState` | `None` | string | readWrite | data-model-generic.xml |
| 60 | `Device.IP.Diagnostics.UDPEchoConfig.EchoPlusEnabled` | `0` | boolean | readWrite | data-model-generic.xml |
| 61 | `Device.IP.Diagnostics.UDPEchoConfig.EchoPlusSupported` | `1` | boolean | readOnly | data-model-generic.xml |
| 62 | `Device.IP.Diagnostics.UDPEchoConfig.Enable` | `0` | boolean | readWrite | data-model-generic.xml |
| 63 | `Device.IP.Diagnostics.UploadDiagnostics.DSCP` | `0` | unsignedInt | readWrite | data-model-generic.xml |
| 64 | `Device.IP.Diagnostics.UploadDiagnostics.DiagnosticsState` | `None` | string | readWrite | data-model-generic.xml |
| 65 | `Device.IP.Diagnostics.UploadDiagnostics.EthernetPriority` | `0` | unsignedInt | readWrite | data-model-generic.xml |
| 66 | `Device.IP.Diagnostics.UploadDiagnostics.UploadTransports` | `HTTP,FTP` | string | readOnly | data-model-generic.xml |
| 67 | `Device.InterfaceStackNumberOfEntries` | `0` | unsignedInt | readOnly | data-model-generic.xml |
| 68 | `Device.ManagementServer.AliasBasedAddressing` | `1` | boolean | readOnly | data-model-generic.xml |
| 69 | `Device.ManagementServer.AutoCreateInstances` | `0` | boolean | readWrite | data-model-generic.xml |
| 70 | `Device.ManagementServer.DefaultActiveNotificationThrottle` | `600` | unsignedInt | readWrite | data-model-generic.xml |
| 71 | `Device.ManagementServer.InstanceMode` | `InstanceNumber` | string | readWrite | data-model-generic.xml |
| 72 | `Device.ManagementServer.NATDetected` | `0` | boolean | readOnly | data-model-generic.xml |
| 73 | `Device.ManagementServer.PeriodicInformEnable` | `0` | boolean | readWrite | data-model-generic.xml |
| 74 | `Device.ManagementServer.PeriodicInformInterval` | `300` | unsignedInt | readWrite | data-model-generic.xml |
| 75 | `Device.ManagementServer.STUNEnable` | `1` | boolean | readWrite | data-model-generic.xml |
| 76 | `Device.ManagementServer.STUNMaximumKeepAlivePeriod` | `80` | int | readWrite | data-model-generic.xml |
| 77 | `Device.ManagementServer.STUNMinimumKeepAlivePeriod` | `30` | unsignedInt | readWrite | data-model-generic.xml |
| 78 | `Device.ManagementServer.STUNServerPort` | `3478` | unsignedInt | readWrite | data-model-generic.xml |
| 79 | `Device.ManagementServer.UpgradesManaged` | `0` | boolean | readWrite | data-model-generic.xml |
| 80 | `Device.Services.STBService.1.Components.AudioOutput.1.CancelMute` | `0` | boolean | readWrite | data-model-generic.xml |
| 81 | `Device.Services.STBService.1.Components.AudioOutput.1.Name` | `The Audio Output Component` | string | readOnly | data-model-generic.xml |
| 82 | `Device.Services.STBService.1.Components.AudioOutputNumberOfEntries` | `1` | unsignedInt | readOnly | data-model-generic.xml |
| 83 | `Device.Services.STBService.1.Components.HDMINumberOfEntries` | `1` | unsignedInt | readOnly | data-model-generic.xml |
| 84 | `Device.Services.STBService.1.Components.VideoDecoderNumberOfEntries` | `1` | unsignedInt | readOnly | data-model-generic.xml |
| 85 | `Device.Services.STBService.1.Components.VideoOutputNumberOfEntries` | `1` | unsignedInt | readOnly | data-model-generic.xml |
| 86 | `Device.Services.STBService.1.Enable` | `1` | boolean | readWrite | data-model-generic.xml |
| 87 | `Device.Services.STBServiceNumberOfEntries` | `1` | unsignedInt | readOnly | data-model-generic.xml |
| 88 | `Device.Time.Chrony.Enable` | `FALSE` | boolean | readWrite | data-model-generic.xml |
| 89 | `Device.Time.Chrony.Makestep` | `1.0,3` | string | readWrite | data-model-generic.xml |
| 90 | `Device.WiFi.AccessPoint.{i}.Enable` | `0` | boolean | readWrite | data-model-generic.xml |
| 91 | `Device.WiFi.AccessPoint.{i}.Security.RadiusServerPort` | `1812` | unsignedInt | readWrite | data-model-generic.xml |
| 92 | `Device.WiFi.AccessPoint.{i}.Security.RekeyingInterval` | `3600` | unsignedInt | readWrite | data-model-generic.xml |
| 93 | `Device.WiFi.AccessPoint.{i}.Status` | `Disable` | string | readOnly | data-model-generic.xml |
| 94 | `Device.WiFi.AccessPoint.{i}.WPS.Enable` | `1` | boolean | readWrite | data-model-generic.xml |
| 95 | `Device.WiFi.EndPoint.{i}.Enable` | `0` | boolean | readWrite | data-model-generic.xml |
| 96 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Enable` | `0` | boolean | readWrite | data-model-generic.xml |
| 97 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Priority` | `0` | unsignedInt | readWrite | data-model-generic.xml |
| 98 | `Device.WiFi.EndPoint.{i}.Profile.{i}.Status` | `Disabled` | string | readOnly | data-model-generic.xml |
| 99 | `Device.WiFi.EndPoint.{i}.Stats.Retransmissions` | `0` | unsignedInt | readOnly | data-model-generic.xml |
| 100 | `Device.WiFi.EndPoint.{i}.Status` | `Disabled` | string | readOnly | data-model-generic.xml |
| 101 | `Device.WiFi.EndPoint.{i}.WPS.Enable` | `1` | boolean | readWrite | data-model-generic.xml |
| 102 | `Device.WiFi.SSID.{i}.Enable` | `0` | boolean | readWrite | data-model-generic.xml |
