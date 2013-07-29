import xml.dom.minidom
import codecs
import os
import stat

def OpenSourceInclude(name, version='Current'):
    return {'root':'OpenSource', 'type':'include', 'name':name, 'version':version}
def OpenSourceLibrary(name, version='Current'):
    return {'root':'OpenSource', 'type':'library', 'name':name, 'version':version}
def ThirdPartyInclude(company, name, version='Current'):
    return {'root':'ThirdParty', 'type':'include', 'company':company, 'name':name, 'version':version}
def ThirdPartyLibrary(company, name, version='Current'):
    return {'root':'ThirdParty', 'type':'library', 'company':company, 'name':name, 'version':version}

include_directories = [
  {'value':'\\Editor\\Build\\include'}
, {'value':'\\Editor\\Core'}
, {'value':'\\Editor\\Core\\YDWEBase'}
, OpenSourceInclude('Aero')
, OpenSourceInclude('APR')
, OpenSourceInclude('APR-Util')
, {'root':'OpenSource', 'name':'Boost', 'value2':'\\1.53'}
, {'root':'OpenSource', 'name':'CImg', 'value2':'\\Current'}
, OpenSourceInclude('Log4CXX')
, OpenSourceInclude('Lua')
, OpenSourceInclude('LuaBind')
, OpenSourceInclude('MiniZip')
, OpenSourceInclude('Stlsoft')
, OpenSourceInclude('StormLib')
, ThirdPartyInclude('Blizzard', 'Storm')
, ThirdPartyInclude('Microsoft', 'Detours')
, {'root':'ThirdParty', 'company':'RadGameTools', 'name':'MSS', 'value2':'\\6.1a\\Includes'}
]

library_directories = [
  OpenSourceLibrary('APR')
, OpenSourceLibrary('APR-Util')
, {'root':'OpenSource', 'name':'Boost', 'value2':'\\1.53\\stage\\lib\\$(PlatformName)'}
, OpenSourceLibrary('Log4CXX')
, OpenSourceLibrary('Lua')
, OpenSourceLibrary('LuaBind')
, OpenSourceLibrary('StormLib')
, OpenSourceLibrary('ZLib')
, {'root':'ThirdParty', 'company':'Blizzard', 'name':'Storm', 'value2':'\\Current\\lib\\Win32\\DebugRelease'}
, ThirdPartyLibrary('Microsoft', 'Detours')
]

def create_inc_lib_directory(config):
    if 'value' in config:
        return config['value']
    
    if  'version' not in config:
        config['version'] = 'Current'
        
    s = '\\..\\..\\..'
    if config['root'] == 'OpenSource':
        s = s + '\\OpenSource'
        s = s + '\\' + config['name']
        if 'value2' in config:
            return s + config['value2']
        s = s + '\\' + config['version']   
    elif config['root'] == 'ThirdParty':
        s = s + '\\ThirdParty'
        s = s + '\\' + config['company']
        s = s + '\\' + config['name']
        if 'value2' in config:
            return s + config['value2']
        s = s + '\\' + config['version']
    else:
        return ''
    
    if config['type'] == 'include':
        s = s + '\\include'
    elif config['type'] == 'library':
        s = s + '\\lib\\$(PlatformName)\\$(ConfigurationName)'
    else:
        return ''
    return s

def get_parent_path(p):
    return os.path.split(p)[0]

def get_root_path():
    return get_parent_path(get_parent_path(get_parent_path(os.getcwd())))

def create_dotdot(n):
    s = ''
    for i in range(n):
        s = s + '\\..'
    return s

def create_include_directories(n, name):
    s = ''
    for c in include_directories:
        if name == ('yd_lua_engine.vcxproj') and ('value' not in c) and (c['name'] == 'Lua'):
            new_c = {'root':'OpenSource', 'type':'include', 'name':'Lua', 'version':'5.2.2'}
            s = s + '$(ProjectDir)' + create_dotdot(n) + create_inc_lib_directory(new_c) + ';'
        else:
            s = s + '$(ProjectDir)' + create_dotdot(n) + create_inc_lib_directory(c) + ';'
    s = s + '%(AdditionalLibraryDirectories)'
    return s
    
def create_library_directories(n, name):
    s = ''
    for c in library_directories:
        s = s + '$(ProjectDir)' + create_dotdot(n) + create_inc_lib_directory(c) + ';'
    s = s + '%(AdditionalLibraryDirectories)'
    return s

def update_vcxproj(n, path):
    doc = xml.dom.minidom.parse(path)
    name = os.path.split(path)[1]
    include_value = create_include_directories(n, name)
    library_value = create_library_directories(n, name)
    for ItemDefinitionGroup in doc.documentElement.getElementsByTagName('ItemDefinitionGroup'):
        
        flag_include = False
        for ClCompile in ItemDefinitionGroup.getElementsByTagName('ClCompile'):
            for AdditionalIncludeDirectories in ClCompile.getElementsByTagName('AdditionalIncludeDirectories'):
                AdditionalIncludeDirectories.childNodes[0].nodeValue = include_value
                flag_include = True                
        if not flag_include:
            ClCompiles = ItemDefinitionGroup.getElementsByTagName('ClCompile')            
            AdditionalIncludeDirectories = doc.createElement('AdditionalIncludeDirectories')
            AdditionalIncludeDirectories.appendChild(doc.createTextNode(include_value)) 
            if len(ClCompiles) == 0:
                ClCompile = doc.createElement('ClCompile')
                ClCompile.appendChild(AdditionalIncludeDirectories)
                ItemDefinitionGroup.appendChild(ClCompile)
            else:
                ClCompiles[0].appendChild(AdditionalIncludeDirectories)
            
        flag_library = False
        for Link in ItemDefinitionGroup.getElementsByTagName('Link'):
            for AdditionalLibraryDirectories in Link.getElementsByTagName('AdditionalLibraryDirectories'):
                AdditionalLibraryDirectories.childNodes[0].nodeValue = library_value
                flag_library = True
        if not flag_library:
            Links = ItemDefinitionGroup.getElementsByTagName('Link')            
            AdditionalLibraryDirectories = doc.createElement('AdditionalLibraryDirectories')
            AdditionalLibraryDirectories.appendChild(doc.createTextNode(library_value)) 
            if len(Links) == 0:
                Link = doc.createElement('Link')
                Link.appendChild(AdditionalLibraryDirectories)
                ItemDefinitionGroup.appendChild(Link)
            else:
                Links[0].appendChild(AdditionalLibraryDirectories)
         
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
