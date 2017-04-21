clear;

% Player
player = 'q';

% If the filename has some extension in the end
extension = '';

% Import data
filename = ['../statistics/stats_pert_' player 'player' extension '.csv'];
data = csvread(filename);

% Game parameters
N_perturb = data(1, 1);
N_games = data(1, 2);
N_rows = data(1, 3);
rows = zeros(N_rows, 1);
for k = 1:N_rows
    rows(k) = data(1, 3+k);
end

% Statistics
stats = data(2:end, 1);

% Display game parameters
fprintf(['\n' player '-player:\n']);
fprintf(['  N_perturb = ' num2str(N_perturb) '\n']);
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
    step = 1/(N_perturb-1);
    
    % Plot
    figure;
    x_vals = 0:step:1;
    plot(x_vals, stats, '*k');
    title(['Win rate (' player '-player)']);
    xlabel('perturb');
    ylabel('win rate');
    ylim([0 1])
end
