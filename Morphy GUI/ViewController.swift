//
//  ViewController.swift
//  Morphy GUI
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {

    
    @IBOutlet weak var mainTextBuffer: NSTextView!;
    
    override func viewDidLoad() {
        super.viewDidLoad()

        mainTextBuffer.isEditable = false;
        mainTextBuffer.font = NSFont(name: "Courier", size: 12);
        mainTextBuffer.string += "\n Morphy\n\n";
        //mainTextBuffer.string  = "\t+--------------------------------------+";
        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }

    @IBAction func printMessageToBuffer(message: NSString) {
        mainTextBuffer.string += "\n\n";
        mainTextBuffer.string += message as String;
        mainTextBuffer.string += "\n\n";
    }
    
    func printTextToBuffer(message: NSString) {
        
    }

}

