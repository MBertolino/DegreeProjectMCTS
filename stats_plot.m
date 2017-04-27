clear;

% Player
player = 'xp';

% If the filename has some extension in the end
extension = '';

% Import data
filename = ['../statistics/stats_' player extension '.csv'];
data = csvread(filename);

% Game parameters
perturb = data(1, 1);
N_vals1 = data(1, 2);
N_vals2 = data(1, 3);
N_games = data(1, 4);
N_rows = data(1, 5);
total_sticks = data(1, 6);
c_min = data(1, 7);
c_max = data(1, 8);

% Statistics
stats = data(2:end, 1:(N_vals2+1));

% Display game parameters
fprintf(['\n' player(1) ' vs ' player(2) ':\n']);
fprintf(['  perturb      = ' num2str(perturb) '\n']);
fprintf(['  N_vals1      = ' num2str(N_vals1) '\n']);
fprintf(['  N_vals2      = ' num2str(N_vals2) '\n']);
fprintf(['  N_games      = ' num2str(N_games) '\n']);
fprintf(['  N_rows       = ' num2str(N_rows) '\n']);
fprintf(['  total_sticks = ' num2str(total_sticks) '\n\n']);

%% Regular plot
if ~strcmp(player, 'pp')
    
    % Data
    N_vals = length(stats);
    step = 1/(N_vals-1);
    
    % x-vector
    x_vals = 0:step:1;
    if player(1) == 'x'
        x_vals = c_min*10.^(x_vals*log10(c_max/c_min));
    end
    
    % Plot
    figure;
    if player(1) ~= 'x'
        plot(x_vals, stats, '*b');
    else
        semilogx(x_vals, stats, '*b');
    end
    grid on;
    title([player(1) ' vs ' player(2)]);
    ylabel(['win rate (' player(2) '-player)']);
    ylim([0 1]);
    switch player(1)
        case 'p'
            xlabel('p-value')
        case 'q'
            xlabel('q-value')
        case 'x'
            xlabel('c')
    end
end

%% Heatmap
if strcmp(player, 'pp')
    
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
    xlabel('p_{2}');
    ylabel('p_{1}');
    
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

