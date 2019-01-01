//
//  AppDelegate.swift
//  Morphy GUI
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    //var morphyHandle = mpl_
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }
    
    
    @IBAction func openFileBrowser(_ sender: AnyObject) {
        
        let dialog = NSOpenPanel();
       
        var openFileName: String = "";

        dialog.title = "Open File..."
        dialog.showsResizeIndicator = true;
        dialog.showsHiddenFiles = false;
        dialog.canChooseDirectories = true;
        dialog.canCreateDirectories = false;
        dialog.allowsMultipleSelection = false;
        dialog.allowedFileTypes = ["txt", "nex"];
        
        if (dialog.runModal() == NSApplication.ModalResponse.OK) {
            let result = dialog.url; // Pathname of the file
            
            if (result != nil) {
                let path = result!.path
                openFileName = path;
            }
            
        } else {
            // User clicked on "Cancel"
            print("ok fine whatever...");
            return
        }
    }

    //@IBOutlet

}

