clear
close all
clc

serialportlist("available")
arduinoObj = serialport("/dev/ttyACM0",115200)
flush(arduinoObj)
configureTerminator(arduinoObj,"CR/LF"); 


arduinoObj.UserData = struct("Data",[],"Count",1)

configureCallback(arduinoObj,"terminator",@readSineWaveData)
clear arduinoObj

%%
%%
flush(arduinoObj)
while true
    % arduinoObj.Terminator
    % readline(arduinoObj);
    configureTerminator(arduinoObj,"CR");
    str = readline(arduinoObj);
    %%[num1, num2, num3] = 
    [num1, remain] = strtok(str,',');
    [num2, remain] = strtok(remain,',');
    [num3, remain] = strtok(remain,',');
    int1 = str2double(num1)
    int2 = str2double(num2)
    int3 = str2double(num3)

    % read(arduinoObj, 1,"int32")
    % fscanf(arduinoObj,'%d')
    % fprintf(s, '%d', data);
    % fprintf(arduinoObj, '%d', data);
end


%%
delete(arduinoObj)


%%


function readSineWaveData(src, ~)

% Read the ASCII data from the serialport object.
data = readline(src);

% Convert the string data to numeric type and save it in the UserData
% property of the serialport object.
src.UserData.Data(end+1) = str2double(data);

% Update the Count value of the serialport object.
src.UserData.Count = src.UserData.Count + 1;

% If 1001 data points have been collected from the Arduino, switch off the
% callbacks and plot the data.
if src.UserData.Count > 1001
    configureCallback(src, "off");
    plot(src.UserData.Data(2:end));
end
end


