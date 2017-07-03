This is a Google Docs script that would allow you to show the folder containing the current document. This is a fixed version of the following script https://webapps.stackexchange.com/a/51115

Setup instruction (copied from the text in the above link): 

Select Tools > Script editor...

If this is your first time editing that document's script, default code will populate the editor's screen. Simply replace the script content with the code in [code.gs](code.gs). Include both the function onOpen() and listParentFolders() listed below.

Save the script in the editor and then 'refresh' the browse window displaying the associated document. A new menu item will appear for the document named Utils. Clicking on the Utils Menu pad will display the menu popup, Show Path (you will be asked to authorize this script). This script will display the directory path as a list of hyperlinks.
