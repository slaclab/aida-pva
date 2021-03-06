global aidapvainitdone;
aidapva;

if isempty(aidapvainitdone)
    %    setupjavapath(strcat(getenv('PHYSICS_TOP'),'/release/aida-pva-client/R1.0.0/lib/aida-pva-client.jar'))
    %    setupjavapath(strcat(getenv('PHYSICS_TOP'),'/release/epics-pvaccess/R1.0.0/lib/epics-pvaccess.jar'))
    setupjavapath(strcat(getenv('PWD'),'/aida-pva-client.jar'))
    setupjavapath(strcat(getenv('PWD'),'/epics-pvaccess.jar'))

    % aida-pva-client imports - imports all methods but not enums, so we need to add enums indivudually later
    import('edu.stanford.slac.aida.client.AidaPvaClientUtils.*');
    import('edu.stanford.slac.aida.client.AidaType.*');

    AIDA_BOOLEAN = [edu.stanford.slac.aida.client.AidaType.AIDA_BOOLEAN];
    AIDA_BYTE = [edu.stanford.slac.aida.client.AidaType.AIDA_BYTE];
    AIDA_CHAR = [edu.stanford.slac.aida.client.AidaType.AIDA_CHAR];
    AIDA_SHORT = [edu.stanford.slac.aida.client.AidaType.AIDA_SHORT];
    AIDA_INTEGER = [edu.stanford.slac.aida.client.AidaType.AIDA_INTEGER];
    AIDA_LONG = [edu.stanford.slac.aida.client.AidaType.AIDA_LONG];
    AIDA_FLOAT = [edu.stanford.slac.aida.client.AidaType.AIDA_FLOAT];
    AIDA_DOUBLE = [edu.stanford.slac.aida.client.AidaType.AIDA_DOUBLE];
    AIDA_STRING = [edu.stanford.slac.aida.client.AidaType.AIDA_STRING];
    AIDA_BOOLEAN_ARRAY = [edu.stanford.slac.aida.client.AidaType.AIDA_BOOLEAN_ARRAY];
    AIDA_BYTE_ARRAY = [edu.stanford.slac.aida.client.AidaType.AIDA_BYTE_ARRAY];
    AIDA_CHAR_ARRAY = [edu.stanford.slac.aida.client.AidaType.AIDA_CHAR_ARRAY];
    AIDA_SHORT_ARRAY = [edu.stanford.slac.aida.client.AidaType.AIDA_SHORT_ARRAY];
    AIDA_INTEGER_ARRAY = [edu.stanford.slac.aida.client.AidaType.AIDA_INTEGER_ARRAY];
    AIDA_LONG_ARRAY = [edu.stanford.slac.aida.client.AidaType.AIDA_LONG_ARRAY];
    AIDA_FLOAT_ARRAY = [edu.stanford.slac.aida.client.AidaType.AIDA_FLOAT_ARRAY];
    AIDA_DOUBLE_ARRAY = [edu.stanford.slac.aida.client.AidaType.AIDA_DOUBLE_ARRAY];
    AIDA_STRING_ARRAY = [edu.stanford.slac.aida.client.AidaType.AIDA_STRING_ARRAY];
    AIDA_TABLE = [edu.stanford.slac.aida.client.AidaType.AIDA_TABLE];

    pvaSetM = @(channel, value) ML(edu.stanford.slac.aida.client.AidaPvaClientUtils.pvaSet(channel, value));

    % Globals above only work when not inside a function so we need to do the same thing inside a function
    % to define these globals for use in any function.  After this you simply need run `aidapva` to
    % import symbols into your scope.
    aidapvafninit

    aidapvainitdone = 1;
    disp 'Aida PVA client initialization completed';
end
