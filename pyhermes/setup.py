from distutils.core import setup, Extension

pyChermes = Extension(
    'hermes', sources = ['pyhermes.c', 'transport.c'], include_dirs=['../include'], library_dirs=['../build'],
    libraries=['hermes_mid_hermes', 'hermes_mid_hermes_ll', 'hermes_credential_store', 'hermes_data_store',
               'hermes_key_store', 'hermes_rpc', 'hermes_common', 'themis', 'soter'])

setup(
    name='hermes',
    version='0.5',
    description='python bindings for Hermes',
    ext_modules=[pyChermes])
