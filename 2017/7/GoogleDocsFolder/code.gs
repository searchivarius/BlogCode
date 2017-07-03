// Based on a solution with a couple of fixes:
// https://webapps.stackexchange.com/a/51115
function onOpen() {


  // Add a menu with some items, some separators, and a sub-menu.
  DocumentApp.getUi().createMenu('Utils')
      .addItem('Show Path', 'listParentFolders')
      .addToUi();
}




function listParentFolders() {

  var theDocument = DocumentApp.getActiveDocument();

  var docID = theDocument.getId();

  var theFile = DriveApp.getFileById(docID);

  var parents = theFile.getParents();

  // No folders
  if ( parents == null || !parents.hasNext()) return;
  
  var folder = parents.next();

  var folderName = folder.getName();

  var folderURL = folder.getUrl();

  var folders = [[folderName,folderURL]];

  while (folderName != "Root"){

     parents = folder.getParents();

     if (parents == null || !parents.hasNext()) break;
     folder = parents.next();

     folderName = folder.getName();

     folderURL = folder.getUrl();

     folders.unshift([folderName,folderURL]);
  }

  var app = UiApp.createApplication().setTitle("Folder Hierarchy").setHeight(250).setWidth(300);

  var grid = app.createGrid(folders.length, 1).setStyleAttribute(0, 0, "width", "300px").setCellPadding(5);

  var indentNum = 0, link;

  for (var fldCntr = 0; fldCntr < folders.length; fldCntr++){

     folderName = folders[fldCntr][0];

     folderURL = folders[fldCntr][1];

     link = app.createAnchor(folderName, folderURL).setId("id_" + fldCntr).setStyleAttribute("font-size", "10px");

     grid.setWidget(indentNum, 0, link);

     indentNum += 1;
  }

  app.add(grid);

  DocumentApp.getUi().showSidebar(app);
}
