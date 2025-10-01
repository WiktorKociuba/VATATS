function Component()
{

}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if(systemInfo.productType === "windows"){
        component.addOperation("CreateShortcut",
                               "@TargetDir@/VATATS.exe",
                               "@DesktopDir@/VATATS.lnk",
                               "workingDirectory=@TargetDir@",
                               "iconPath=@TargetDir@/VATATS.exe",
                               "description=Virtual Aircraft Tracking And Toolbox System");
        component.addOperation("CreateShortcut",
                               "@TargetDir@/VATATS.exe",
                               "@StartMenuDir@/VATATS.lnk");
        component.addOperation("CreateShortcut",
                               "@TargetDir@/VATATSMaintenanceTool.exe",
                               "@StartMenuDir@/Uninstall VATATS.lnk",
                               "--uninstall");
        component.addOperation("RegisterFileType",
                               "vatats",
                               "@TargetDir@/VATATSMaintenanceTool.exe --uninstall",
                               "VATATS Application",
                               "application/vnd.vatats",
                               "Software\\VATATS");
    }
}