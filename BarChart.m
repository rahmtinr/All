OutputFiles = dir('../Output_All');
numfids = length(OutputFiles);
vals = cell(1, 100);
for K = 1 : numfids
    if length(OutputFiles(K).name) < 4
        continue
    end
    if strcmpi(OutputFiles(K).name(end-3 : end), '.txt')
        if strcmpi(OutputFiles(K).name(1:1),'i')
            continue;
        end
        if length(findstr(OutputFiles(K).name, 'has_error')) == 0
            [X, Y] = textread(strcat('../Output_All/',OutputFiles(K).name) ...
                , '%s %f', 100);
            Z = zeros(length(X),1);
        else
            [X, Y, Z] = textread(strcat('../Output_All/',OutputFiles(K).name) ...
                , '%s %f %f', 100);
        end
        figure1 = figure;
        if sum(Y)> 5
            plot(Y);
            hold on;
            histogram = errorbar(Y, Z, 'bx');
            hc = get(histogram, 'Children');
            set(hc(1),'color','b'); %// data
            set(hc(2),'color','r'); %// error bar
            set(gca, 'XTickLabel', X, 'XTick',1:numel(X),'FontSize',5)
            rotateXLabels( gca(), 90 )
            saveas(figure1, strcat('../Output_All/',...
                OutputFiles(K).name(1 : end-4), '.jpg'))
        end
        close(figure1)
    end
end

% OutputFiles = dir('../Output_FineFoods/PerItem/')
% length(OutputFiles)
% numfids = length(OutputFiles);
% vals = cell(1, 100);
% for K = 1 : numfids
%     if length(OutputFiles(K).name) < 4
%         continue
%     end
%     if strcmpi(OutputFiles(K).name(end-3 : end), '.txt')
%         OutputFiles(K).name
%         [X, Y] = textread(strcat('../Output_FineFoods/PerItem/',...
%             OutputFiles(K).name), '%s %d', 100);
%        figure1 = figure;
%        if sum(Y)> 10
%             bar(Y)
%             set(gca, 'XTickLabel', X, 'XTick',1:numel(X),'FontSize',8)
%             rotateXLabels( gca(), 90 )
%             saveas(figure1, strcat('../Output_FineFoods/PerItem/',...
%                 OutputFiles(K).name(1 : end-4), '.jpg'))
%        end
%         close(figure1)
%     end
% end