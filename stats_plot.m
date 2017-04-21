clear;

% Player
player = 'q';

% If the filename has some extension in the end
extension = '';

% Import data
filename = ['../statistics/stats_' player 'player' extension '.csv'];
data = csvread(filename);

% Game parameters
perturb = data(1, 1);
N_vals1 = data(1, 2);
N_vals2 = data(1, 3);
N_games = data(1, 4);
N_rows = data(1, 5);
rows = zeros(N_rows, 1);
for k = 1:N_rows
    rows(k) = data(1, 5+k);
end

% Statistics
stats = data(2:end, 1:(N_vals2+1));

% Display game parameters
fprintf(['\n' player '-player:\n']);
fprintf(['  perturb = ' num2str(perturb) '\n']);
fprintf(['  N_vals1 = ' num2str(N_vals1) '\n']);
fprintf(['  N_vals2 = ' num2str(N_vals2) '\n']);
fprintf(['  N_games = ' num2str(N_games) '\n']);
fprintf(['  N_rows  = ' num2str(N_rows) '\n\n']);
fprintf('Initial board:\n');
for i = 1:N_rows
    fprintf('  ');
    for j = 1:rows(i)
        fprintf('|');
    end
    fprintf('\n');
end
fprintf('\n');

%% Regular plot
if player ~= 'p'
    
    % Data
    N_vals = length(stats);
    step = 1/(N_vals-1);
    
    % Plot
%     figure;
hold on;
    x_vals = 0:step:1;
    plot(x_vals, stats, '*k');
    title(['Win rate (' player '-player)']);
    xlabel('p-value');
    ylabel('win rate');
    ylim([0 1])
end

%% Heatmap
if player == 'p'
    
    % Data
    stats = 1 - stats;
    stats_blurred = imgaussfilt(stats, 30);
    N_vals = size(stats, 2);
    
    % Number of ticks
    N_ticks = 10;
    
    % Plot the heatmap
    figure;
    colormap('jet');
    imagesc(flipud(stats));
    colorbar;
    title('Win rate of player 1');
    xlabel('p2');
    ylabel('p1');
    
    % Contour lines
    hold on;
    contour(flipud(stats_blurred), 'black');
    
    % Tick marks
    tick_step = N_vals/N_ticks;
    ax = gca;
    ax.XTick = [1 tick_step:tick_step:N_vals];
    ax.XTickLabel = [0 ax.XTick(2:end)/N_vals];
    ax.YTick = [1 tick_step:tick_step:N_vals];
    ax.YTickLabel = fliplr([0 ax.YTick(2:end)/N_vals]);
end

