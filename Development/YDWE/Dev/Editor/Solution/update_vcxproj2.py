import xml.dom.minidom
import codecs
import os
import stat

def get_parent_path(p):
    return os.path.split(p)[0]

def get_root_path():
    return get_parent_path(get_parent_path(get_parent_path(os.getcwd())))

def update_vcxproj(n, path):
    name = os.path.split(path)[1]
    doc = xml.dom.minidom.parse(path)
    Project = doc.documentElement
    for ItemDefinitionGroup in Project.getElementsByTagName('ItemDefinitionGroup'):
        for ClCompile in ItemDefinitionGroup.getElementsByTagName('ClCompile'):
            if name == 'yd_lua_engine.vcxproj':
                for AdditionalIncludeDirectories in ClCompile.getElementsByTagName('AdditionalIncludeDirectories'):
                    AdditionalIncludeDirectories.childNodes[0].nodeValue = r'$(YDWE)\..\..\..\OpenSource\Lua\5.2.2\include;%(AdditionalIncludeDirectories)'
            else:
                for AdditionalIncludeDirectories in ClCompile.getElementsByTagName('AdditionalIncludeDirectories'):
                    ClCompile.removeChild(AdditionalIncludeDirectories)

            for ResourceCompile in ItemDefinitionGroup.getElementsByTagName('ResourceCompile'):
                ItemDefinitionGroup.removeChild(ResourceCompile)
        for Link in ItemDefinitionGroup.getElementsByTagName('Link'):
            for AdditionalLibraryDirectories in Link.getElementsByTagName('AdditionalLibraryDirectories'):
                Link.removeChild(AdditionalLibraryDirectories)
                
    for PropertyGroup in Project.getElementsByTagName('PropertyGroup'):
        for OutDir in PropertyGroup.getElementsByTagName('OutDir'):
            PropertyGroup.removeChild(OutDir) 
        for IntDir in PropertyGroup.getElementsByTagName('IntDir'):
            PropertyGroup.removeChild(IntDir)
            
    for ImportGroup in Project.getElementsByTagName('ImportGroup'):
        if 'PropertySheets' == ImportGroup.attributes['Label'].value:
            has_ydwe_build = False
            value_ydwe_build = r"$([MSBuild]::GetDirectoryNameOfFileAbove($(MSBuildThisFileDirectory), build.root))\Editor\Core\Solution\ydwe_build.props"
            for Import in ImportGroup.getElementsByTagName('Import'):
                if Import.attributes['Project'].value == value_ydwe_build:
                    has_ydwe_build = True
                    break
            if not has_ydwe_build:
                Import = doc.createElement('Import')
                Import.setAttribute('Project', value_ydwe_build)
                ImportGroup.appendChild(Import)
    
    os.chmod(path, stat.S_IWRITE)   
    writer = codecs.lookup('utf-8')[3](file(path, 'wb'))
    doc.writexml(writer, encoding = 'utf-8')
    writer.close()

def vcxproj_generators(path, dep = 0):
    for name in os.listdir(path):
        file_path = os.path.join(path, name)
        if os.path.isfile(file_path):
            if os.path.splitext(file_path)[1] == '.vcxproj':
                if os.path.split(file_path)[1] != 'Template.vcxproj':
                    yield dep, file_path
        elif os.path.isdir(file_path):
            for i, p in vcxproj_generators(file_path, dep + 1):
                yield i, p

for n, p in vcxproj_generators(get_root_path()):
    print p
    update_vcxproj(n, p)
    

'''
doc = minidom.parse('YDWEInject.vcxproj')
root = doc.documentElement
for ProjectReference in root.getElementsByTagName('ProjectReference'):
    print ProjectReference.attributes['Include'].value
    print ProjectReference.getElementsByTagName('Project')[0].childNodes[0].nodeValue
'''
