%% Test read ArduinoObj

clear
close all
clc

arduino_COM = "COM3";

COMS_str = serialportlist("available");

arduinoObj = internal.Serialport.empty();

for i = 1:size(COMS_str,2)
    if strcmp(COMS_str(i),arduino_COM) == 1
        arduinoObj = serialport(COMS_str(i),115200);
    end
end

if isempty(arduinoObj)
    disp("No serial port " + arduino_COM + " detected")
    return
end

flush(arduinoObj)
configureTerminator(arduinoObj,"CR/LF"); 


arduinoObj.UserData = struct("Data",[],"Count",1)

configureCallback(arduinoObj,"terminator",@readSineWaveData)
% clear arduinoObj

%% ArduinoObj read 4 values

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


%% ArduinoObj read a package of 96 values

flush(arduinoObj)

measured_time = 0;
plot_data = [];
plot_lines = gobjects(4);

sensor_corr_tab = [12.5, 14, 21.5, 24, 27, 28.5, 33.5, 36, 42.5, 45.5, 52.5, 58, 64, 69, 74.5, 80, 84.5, 88.5, 94.5, 99, 103, 108, 112.5, 116;
                   28, 29, 30.5, 32, 36, 38.5, 42, 45, 51, 56, 62, 66, 71.5, 76, 81.5, 86, 91, 97, 101, 106.5, 111, 115, 119, 124;
                   1, 2.5, 6, 11, 14, 18.5, 25, 29, 33, 37, 43, 50, 53.5, 59, 64, 68, 72.5, 78, 82, 83, 88.5, 90, 94, 99;
                   3, 6, 16.5, 21.5, 24, 26, 29, 33, 36, 41, 43, 48, 53, 58, 61.5, 67, 71.5, 78, 82, 87, 91.5, 95, 99, 103.5];

tic

while true
    dataset_time = toc;
    tic
    % arduinoObj.Terminator
    % readline(arduinoObj);
    configureTerminator(arduinoObj,"CR");
    str_data = readline(arduinoObj);
    % str_data2 = extractBetween(str_data,2,strlength(str_data));
    %%[num1, num2, num3] = 
    str_data_spl = strsplit(str_data,";");

    str_measure = strings(size(str_data_spl,2),4);

    for i = 1:size(str_data_spl,2)
        % try
        str_measure(i,:) = strsplit(str_data_spl(i),",");
        % catch 
        %     size_dataset = size(split(str_data_spl(i),","),1);
        %     str_m = strsplit(str_data_spl(i),",");
        %     for j = size_dataset:4
        %         str_m(j) = "NaN";
        %     end
        %     str_measure(i,:) = str_m;
        % end
    end

    for i = 1:size(str_data_spl,2)
        fprintf(string(i) + ": ");
        for j = 1:size(str_measure(i,:),2)
             fprintf("VL" + string(j) + ": " + str_measure(i,j) + ", ");
        end
        fprintf("\n");
    end

    dist_measures = str2double(str_measure) - 2;
    dist_corr_measures = zeros(size(dist_measures));

    
    for i = 1:4
        dist_corr_measures(:,i) = interp1(sensor_corr_tab(i,:),1:size(sensor_corr_tab,2),dist_measures(:,i))*5;
    end


    measured_time = measured_time + dataset_time;
    disp("time: " + string(dataset_time))
    disp("frec: " + string(1/dataset_time))

    plot_data = vertcat(plot_data,dist_corr_measures);
    delay_time = 1e-3;%dataset_time/(size(dist_measures,1)+1);
    disp("delay_time: " + string(delay_time))

    if size(plot_data,1) > 500
        plot_data = plot_data(end-500:end,:);
    end

    figure(1)
    clf
    sgtitle("Mediciones de distancia de los amortiguadores");

    if size(plot_data,1) < 500
        ind1 = size(plot_data,1) - size(dist_corr_measures,1) + 1;
        if ind1 > 1
            for i = 1:4
                subplot(2,2,i)
                plot_lines(i) = plot(plot_data(1:ind1,i),'LineWidth',2,'Marker',".");
                ylim([0 125]);
                grid on
                grid minor  
            end
        end

        ind1 = ind1 + 1;

        for j = ind1:size(plot_data,1)
            for i = 1:4
                subplot(2,2,i)
                plot_lines(i) = plot(plot_data(1:j,i),'LineWidth',2,'Marker',".");
                ylim([0 125]);
                grid on
                grid minor
            end
            % pause(delay_time)
        end
    else
        for j = 1:size(dist_corr_measures,1)
            for i = 1:4
                subplot(2,2,i)
                ind1 = j;
                ind2 = size(plot_data,1) - size(dist_corr_measures,1) + j;
                plot_lines(i) = plot(plot_data(ind1:ind2,i),'LineWidth',2,'Marker',".");
                ylim([0 125]);
                grid on
                grid minor
            end
            % pause(delay_time)
        end
    end
end

%%
% delete(arduinoObj)
clear arduinoObj

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


